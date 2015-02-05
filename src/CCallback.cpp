#include "CCallback.hpp"

#include "format.h"


void CCallbackHandler::Process()
{
	Callback_t callback;
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

						if (amx_address < 0)
							amx_address = tmp_addr;
					}
					callback->m_Params.pop();
				}

				callback->CallPreExecute();

				amx_Exec(amx, NULL, cb_idx);
				if(amx_address >= 0)
					amx_Release(amx, amx_address);

				callback->CallPostExecute();

				break;
			}
		}

		callback.reset();
	}
}

Callback_t CCallbackHandler::Create(string name, string format,
	AMX* amx, cell* params, const cell param_offset)
{
	if (name.empty())
		return nullptr;


	decltype(CCallback::m_Params) param_list;
	if (format.empty() == false)
	{
		if (amx == nullptr || params == nullptr 
			|| static_cast<cell>(params[0] / sizeof(cell)) < param_offset)
			return nullptr;


		cell param_idx = 0;
		cell *address_ptr = nullptr;

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
				CCallbackHandler::Get()->ForwardError(
					EErrorType::CALLBACK_ERROR, 1,
					fmt::format("unrecognized format specifier '{}' in callback \"{}({})\"", c, name, format));
				return nullptr;
			}
			param_idx++;
		}
	}
	return std::make_shared<CCallback>(name, std::move(param_list));
}
