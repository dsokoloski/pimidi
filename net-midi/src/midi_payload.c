#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "midi_payload.h"
#include "utils.h"

void midi_payload_destroy( midi_payload_t **payload )
{
	if( ! payload ) return;
	if( ! *payload ) return;

	(*payload)->buffer = NULL;

	if( (*payload)->header )
	{
		FREENULL( (void **)&((*payload)->header) );
	}

	FREENULL( (void **)payload );
}

void payload_reset( midi_payload_t **payload )
{
	if( ! payload ) return;
	if( ! *payload ) return;

	(*payload)->buffer = NULL;

	if( (*payload)->header )
	{
		(*payload)->header->B = 0;
		(*payload)->header->J = 0;
		(*payload)->header->Z = 0;
		(*payload)->header->P = 0;
		(*payload)->header->len = 0;
	}
}

midi_payload_t * midi_payload_create( void )
{
	midi_payload_t *payload = NULL;

	payload = ( midi_payload_t * )malloc( sizeof( midi_payload_t ) );

	if( ! payload ) return NULL;

	payload->header = ( midi_payload_header_t *)malloc( sizeof( midi_payload_header_t ) );
	if( ! payload->header )
	{
		free( payload );
		return NULL;
	}
	payload->buffer = NULL;

	payload_reset( &payload );

	return payload;
}

void payload_toggle_b( midi_payload_t *payload )
{
	if(! payload) return;

	payload->header->B ^= 1;
}

void payload_toggle_j( midi_payload_t *payload )
{
	if( ! payload ) return;

	payload->header->J ^= 1;
}

void payload_toggle_z( midi_payload_t *payload )
{
	if( ! payload ) return;

	payload->header->Z ^=1;
}

void payload_toggle_p( midi_payload_t *payload )
{
	if( ! payload ) return;

	payload->header->P ^= 1;
}

void payload_set_buffer( midi_payload_t *payload, char *buffer , uint16_t buffer_size)
{
	if( ! payload ) return;

	payload->header->len = buffer_size;
	payload->buffer = buffer;
}


void payload_dump( midi_payload_t *payload )
{
	if( ! payload ) return;
	if( ! payload->header ) return;

	fprintf( stderr, "MIDI Payload\n");
	fprintf( stderr, "B=%d\n", payload->header->B);
	fprintf( stderr, "J=%d\n", payload->header->J);
	fprintf( stderr, "Z=%d\n", payload->header->Z);
	fprintf( stderr, "P=%d\n", payload->header->P);
	fprintf( stderr, "Payload length=%u\n", payload->header->len);
	fprintf( stderr, "--end\n");
}

void payload_pack( midi_payload_t *payload, unsigned char **buffer, size_t *buffer_size)
{
	uint8_t temp_header;
	unsigned char *p = NULL;

	*buffer = NULL;
	*buffer_size = 0;

	if( ! payload ) return;

	if( ! payload->buffer ) return;
	if( ! payload->header ) return;

	payload_dump( payload );

	*buffer_size = 1 + payload->header->len + (payload->header->len > 15 ? 1 : 0);
	*buffer = (unsigned char *)malloc( *buffer_size );

	if( ! *buffer ) return;

	p = *buffer;

	if( payload->header->B) temp_header |= PAYLOAD_HEADER_B;
	if( payload->header->J) temp_header |= PAYLOAD_HEADER_J;
	if( payload->header->Z) temp_header |= PAYLOAD_HEADER_Z;
	if( payload->header->P) temp_header |= PAYLOAD_HEADER_P;

	*p = temp_header;

	if( payload->header->len <= 15 )
	{
		*p |= ( payload->header->len & 0x0f );
		p++;
	} else {
		temp_header |= (payload->header->len & 0x0f00 ) >> 8;
		*p = temp_header;
		p++;
		*p =  (payload->header->len & 0x00ff);
		p++;
	}

	memcpy( p, payload->buffer, payload->header->len );

	hex_dump( *buffer, *buffer_size );
}