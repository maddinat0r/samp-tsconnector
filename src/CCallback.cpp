#include "CCallback.h"


boost::lockfree::spsc_queue<
		CCallback *,
		boost::lockfree::fixed_sized<true>,
		boost::lockfree::capacity<32678>
	> CCallback::m_CallbackQueue;
unordered_set<AMX *> CCallback::m_AmxList;


void CCallback::Process()
{
	CCallback *callback = NULL;
	while(m_CallbackQueue.pop(callback)) 
	{
		for (unordered_set<AMX *>::iterator i = m_AmxList.begin(), end = m_AmxList.end(); i != end; ++i) 
		{
			int cb_idx;
			AMX *amx = (*i);

			if (amx_FindPublic(amx, callback->Name.c_str(), &cb_idx) == AMX_ERR_NONE) 
			{
				cell amx_address = -1;
				while(!callback->Params.empty()) 
				{
					cell tmp_addr;
					amx_PushString(amx, &tmp_addr, NULL, callback->Params.top().c_str(), 0, 0);
					callback->Params.pop();
					if(amx_address == -1)
						amx_address = tmp_addr;
				}

				amx_Exec(amx, NULL, cb_idx);
				if(amx_address >= NULL)
					amx_Release(amx, amx_address);

				break;
			}
		}

		delete callback;
	}
}
