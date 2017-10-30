/* vim: set tw=78 noet si sw=8 ts=8: */
/*!
 * OpenThread NTP Client
 * (C) 2017 VRT Systems <http://www.vrt.com.au>.
 *
 * Based on David Lettier's NTP client
 * (C) 2014 David Lettier <http://www.lettier.com>.
 * Code used with permission.
 */

#include <stdint.h>
#include <sys/time.h>
#include <openthread/types.h>
#include <openthread/ip6.h>
#include <openthread/udp.h>

/*!
 * Standard NTP port number.
 */
#define NTP_CLIENT_DEFAULT_PORT	(123)

/*! Structure that defines the 48 byte NTP packet protocol. */
struct ntp_packet_t {
	/*! Only two bits. Leap indicator. */
	unsigned li	: 2;
	/*! Only three bits. Version number of the protocol. */
	unsigned vn	: 3;
	/*! Only three bits. Mode. Client will pick mode 3 for client. */
	unsigned mode	: 3;

	/*! Eight bits. Stratum level of the local clock. */
	uint8_t stratum;
	/*! Eight bits. Maximum interval between successive messages. */
	uint8_t poll;
	/*! Eight bits. Precision of the local clock. */
	uint8_t precision;

	/*! 32 bits. Total round trip delay time. */
	uint32_t rootDelay;
	/*! 32 bits. Max error aloud from primary clock source. */
	uint32_t rootDispersion;
	/*! 32 bits. Reference clock identifier. */
	uint32_t refId;

	/*! 32 bits. Reference time-stamp seconds. */
	uint32_t refTm_s;
	/*! 32 bits. Reference time-stamp fraction of a second. */
	uint32_t refTm_f;

	/*! 32 bits. Originate time-stamp seconds. */
	uint32_t origTm_s;
	/*! 32 bits. Originate time-stamp fraction of a second. */
	uint32_t origTm_f;

	/*! 32 bits. Received time-stamp seconds. */
	uint32_t rxTm_s;
	/*! 32 bits. Received time-stamp fraction of a second. */
	uint32_t rxTm_f;

	/*!
	 * 32 bits and the most important field the client cares
	 * about. Transmit time-stamp seconds.
	 */
	uint32_t txTm_s;
	/*! 32 bits. Transmit time-stamp fraction of a second. */
	uint32_t txTm_f;
};

/*!
 * NTP client state.  Due to the asynchronous nature of the OpenThread network
 * APIs, the underlying state of a request needs to be handled in the
 * following struct.
 */
struct ntp_client_t {
	/*! OpenThread instance */
	otInstance*		instance;

	/*! UDP socket */
	otUdpSocket		socket;

	/*! NTP packet payload */
	struct ntp_packet_t	packet;

	/*! Received timestamp information */
	struct timeval		tv;

	/*! Result state */
	otError			error;

	/*! Timeout remaining */
	uint16_t		timeout;

	/*! Client state */
	uint8_t			state;
};

/*! Client is being initialised */
#define NTP_CLIENT_INIT		(0x00)
/*! Client has sent the request and is waiting */
#define NTP_CLIENT_SENT		(0x20)
/*! Client has received a reply from the NTP server. */
#define NTP_CLIENT_RECV		(0xa0)
/*! Client received truncated data */
#define NTP_CLIENT_ERR_TRUNC	(0xe0)
/*! Client has processed the reply and is now done. */
#define NTP_CLIENT_DONE		(0xf0)
/*! Client had an internal error. */
#define NTP_CLIENT_INT_ERR	(0xf1)
/*! Client did not receive a reply and has timed out. */
#define NTP_CLIENT_TIMEOUT	(0xff)

/*!
 * Initiate a poll of an NTP server.
 *
 * @param[inout]	instance	OpenThread instance to use for this
 * 					client's context.
 * @param[inout]	ntp_client	NTP client instance
 * @param[in]		addr		IPv6 address of NTP server
 * @param[in]		port		Port number of NTP server
 * @param[in]		ttl		Message time-to-live
 */
otError ntp_client_begin(otInstance* instance,
		struct ntp_client_t* const ntp_client,
		const otIp6Address addr, uint16_t port,
		uint8_t ttl);

/*!
 * Process the state of the NTP client.  This should be called in a loop.
 */
void ntp_client_process(struct ntp_client_t* const ntp_client);
