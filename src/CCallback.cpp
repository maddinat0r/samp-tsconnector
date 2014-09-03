#include "CCallback.h"


void CCallbackHandler::Process()
{
	CCallback *callback = nullptr;
	while (m_Queue.pop(callback))
	{
		for (auto amx : m_AmxList) 
		{
			int cb_idx;
			if (amx_FindPublic(amx, callback->m_Name.c_str(), &cb_idx) == AMX_ERR_NONE) 
			{
				cell amx_address = -1;
				while(callback->m_Params.empty() == false) 
				{
					auto &param = callback->m_Params.top();
					if (param.type() == typeid(cell))
					{
						amx_Push(amx, boost::get<cell>(param));
					}
					else
					{
						cell tmp_addr;
						amx_PushString(amx, &tmp_addr, NULL, 
							boost::get<string>(param).c_str(), 0, 0);

						if (amx_address < NULL)
							amx_address = tmp_addr;
					}
					callback->m_Params.pop();
				}

				callback->CallPreExecute();

				amx_Exec(amx, NULL, cb_idx);
				if(amx_address >= NULL)
					amx_Release(amx, amx_address);

				callback->CallPostExecute();

				break;
			}
		}

		delete callback;
	}
}

CCallback *CCallbackHandler::Create(string name, string format, 
	AMX* amx, cell* params, const cell param_offset)
{
	if (amx == nullptr || params == nullptr || name.empty() || format.empty())
		return nullptr;

	if ((params[0]/sizeof(cell)) < param_offset)
		return nullptr;


	cell param_idx = 0;
	cell *address_ptr = nullptr;
	decltype(CCallback::m_Params) param_list;

	for (auto &c : format)
	{
		switch (c)
		{
		case 'd':
		case 'i':
		case 'f':
		case 'b':
			amx_GetAddr(amx, params[param_offset + param_idx], &address_ptr);
			param_list.push(*address_ptr);
			break;
		case 's':
			param_list.push(amx_GetCppString(amx, params[param_offset + param_idx]));
			break;
		default:
			logprintf(">> plugin.TSConnector: Error while creating callback structure: \
					  (#1) Unrecognized format specifier '%c'", c);
			return nullptr;
		}
		param_idx++;
	}

	return new CCallback(name, std::move(param_list));
}
