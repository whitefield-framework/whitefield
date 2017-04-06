#ifndef	_AIRLINE_H_
#define	_AIRLINE_H_

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"

namespace ns3
{
	class Airline : public Application
	{
		public:
			static TypeId GetTypeId();
			Airline() {
			};
			virtual ~Airline() {
			};
		private:
			/**
			 * \brief Start the application.
			 */
			virtual void StartApplication ();

			/**
			 * \brief Stop the application.
			 */
			virtual void StopApplication ();
			uint32_t m_xyz;	//delete in the future...
			void tx(const uint8_t *pBuf, const size_t buflen);
	};
}

#endif //	_AIRLINE_H_
