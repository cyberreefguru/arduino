/****************************************************************************
 * Copyright (C) 2004 Alex Shepherd
 * 
 * Portions Copyright (C) Digitrax Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *****************************************************************************
 * 
 * IMPORTANT:
 * 
 * Some of the message formats used in this code are Copyright Digitrax, Inc.
 * and are used with permission as part of the EmbeddedLocoNet project. That
 * permission does not extend to uses in other software products. If you wish
 * to use this code, algorithm or these message formats outside of
 * EmbeddedLocoNet, please contact Digitrax Inc, for specific permission.
 * 
 * Note: The sale any LocoNet device hardware (including bare PCB's) that
 * uses this or any other LocoNet software, requires testing and certification
 * by Digitrax Inc. and will be subject to a licensing agreement.
 * 
 * Please contact Digitrax Inc. for details.
 * 
 *****************************************************************************
 * 
 * Title :   LocoNet Buffer Source Code file
 * Author:   Alex Shepherd <kiwi64ajs@sourceforge.net>
 * Date:     13-Feb-2004
 * Software:  AVR-GCC
 * Target:    AtMega8
 * 
 * DESCRIPTION
 * This module provides functions that manage the receiving of
 * LocoNet packets.
 * 
 * 	As bytes are received from the LocoNet, they are stored in a circular
 * 	buffer and after a valid packet has been received it can be read out.
 * 
 * 	Statistics of packets and errors maintained.
 * 
 * 	Any invalid packets that are received are discarded and the stats are
 * 	updated approproately.
 * 
 *****************************************************************************/

#include <string.h>
#include <avr/interrupt.h>
#include "LnBuff.h"

#define		LN_BUF_OPC_WRAP_AROUND	(uint8_t)0x00		// Special character to indicate a buffer wrap
#define		LN_CHECKSUM_SEED        (uint8_t)0xFF

void initLnBuf( LnBuf *buffer )
{
  memset( buffer, 0, sizeof( LnBuf ) ) ;
}

lnMsg *recvLnMsg( LnBuf *buffer )
{
  uint8_t	newByte ;
  uint8_t  bGotNewLength ;
  uint8_t	lastWriteIndex ;
  uint8_t	tempSize ;
  lnMsg *tempMsg ;

  while( buffer->readIndex != buffer->writeIndex )
  {

    newByte = buffer->buf[ buffer->readIndex ] ;

    // Check if this is the beginning of a new packet
    if( newByte & (uint8_t)0x80 )
    {
      // if the ReadPacket index is not the same as the Read index then we have received the
      // start of the next packet without completing the previous one which is an error
      if( buffer->readPacketIndex != buffer->readIndex )
        buffer->stats.rxErrors++ ;

      buffer->readPacketIndex = buffer->readIndex ;
      buffer->checkSum = LN_CHECKSUM_SEED ;
      bGotNewLength = 0 ;
      buffer->readExpLen = ( ( newByte & (uint8_t)0x60 ) == (uint8_t)0x60 ) ? (uint8_t)0 : ( ( newByte & (uint8_t)0x60 ) >> (uint8_t)4 ) + (uint8_t)2 ;
      if (buffer->readExpLen != 0)  // fixed length opcode found?
      {
        bGotNewLength = 1 ;
      }
    }
    // If the Expected Length is 0 and the newByte is not an Command OPC code, then it must be
    // the length byte for a variable length packet
    else 	if( buffer->readExpLen == 0 )
    {
      buffer->readExpLen = newByte ;
      bGotNewLength = 1 ;
    }
    else 
    {
      bGotNewLength = 0 ;
    }

    // if this is the first time we know how long the current packet is then
    // make sure there is enough space left in the buffer to hold the packet
    // without wrapping 
    if( bGotNewLength )
    {
      if( ( buffer->readPacketIndex + buffer->readExpLen ) > LN_BUF_SIZE )
      {
        tempSize = LN_BUF_SIZE - buffer->readPacketIndex ;

        // The packet won't fit in the remaing part of the buffer without wrapping
        // so we need to disable interrupts, update WriteIndex, enable interrupts,
        // move all the data and then fix the ReadIndexes.
        cli();
        // Take a copy of the WriteIndex for later when we move the data
        lastWriteIndex = buffer->writeIndex ;

        if( buffer->writeIndex > buffer->readIndex )
          buffer->writeIndex = buffer->writeIndex - buffer->readPacketIndex ;
        else
          buffer->writeIndex = buffer->writeIndex + tempSize ;

        // Enable interrupts again so we can receive more data etc
        sei();

        // First check if we have to move new data at the buginning of
        // the buffer to make room for the data at the end of the buffer
        if( lastWriteIndex < buffer->readIndex )
        {
          memcpy( buffer->buf + tempSize, buffer->buf, lastWriteIndex ) ;

          // Now move the data at the end of the buffer to the beginning
          memcpy( buffer->buf, buffer->buf + buffer->readPacketIndex, tempSize ) ;
        }
        else
        {
          // copy only already received part of packet
          tempSize = lastWriteIndex - buffer->readPacketIndex;

          // Now move the data at the end of the buffer to the beginning
          memcpy( buffer->buf, buffer->buf + buffer->readPacketIndex, tempSize ) ;
        }

        // Now fix up the ReadIndexes
        buffer->readIndex = buffer->readIndex - buffer->readPacketIndex ;
        buffer->readPacketIndex = 0 ;
      }
    }

    // Set the return packet pointer to NULL first
    tempMsg = NULL ;

    // Advance the ReadIndex and ignore the wrap around until we have calculated the 
    // current packet length
    buffer->readIndex++;

    // Calculate the current packet length
    tempSize = buffer->readIndex - buffer->readPacketIndex ;

    // Check the ReadIndex for wrap around and reset if necessary
    if( buffer->readIndex >= LN_BUF_SIZE )
      buffer->readIndex = 0 ;

    // Do we have a complete packet
    if( tempSize == buffer->readExpLen )
    {
      // Check if we have a good checksum
      if( buffer->checkSum == newByte )
      {
        // Set the return packet pointer
        tempMsg = (lnMsg*) (buffer->buf + buffer->readPacketIndex) ;
        buffer->stats.rxPackets++ ;
      }
      else
        buffer->stats.rxErrors++ ;

      // Whatever the case advance the ReadPacketIndex to the beginning of the
      // next packet to be received
      buffer->readPacketIndex = buffer->readIndex ;

      if( tempMsg != NULL )
        return tempMsg ;

    }

    // Packet not complete so add the current byte to the checksum
    buffer->checkSum ^= newByte ;
  }

  return NULL ;
}

LnBufStats *getLnBufStats( LnBuf *buffer )
{
  return &(buffer->stats) ;
}

uint8_t getLnMsgSize( volatile lnMsg * msg )
{
  return ( ( msg->sz.command & (uint8_t)0x60 ) == (uint8_t)0x60 ) ? msg->sz.mesg_size : ( ( msg->sz.command & (uint8_t)0x60 ) >> (uint8_t)4 ) + 2 ;
}
