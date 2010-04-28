/* ====================================================================================================================

	The copyright in this software is being made available under the License included below.
	This software may be subject to other third party and 	contributor rights, including patent rights, and no such
	rights are granted under this license.

	Copyright (c) 2010, SAMSUNG ELECTRONICS CO., LTD. and BRITISH BROADCASTING CORPORATION
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted only for
	the purpose of developing standards within the Joint Collaborative Team on Video Coding and for testing and
	promoting such standards. The following conditions are required to be met:

		* Redistributions of source code must retain the above copyright notice, this list of conditions and
		  the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
		  the following disclaimer in the documentation and/or other materials provided with the distribution.
		* Neither the name of SAMSUNG ELECTRONICS CO., LTD. nor the name of the BRITISH BROADCASTING CORPORATION
		  may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * ====================================================================================================================
*/

/** \file			TComBitStream.cpp
    \brief		class for handling bitstream
*/

#include "TComBitStream.h"

// ====================================================================================================================
// Constructor / destructor / create / destroy
// ====================================================================================================================

Void TComBitstream::create( UInt uiSize )
{
  m_apulStreamPacketBegin = new UInt[uiSize];
  m_uiBufSize       = uiSize;
  m_uiBitSize       = 0;
  m_iValidBits      = 32;

  m_ulCurrentBits   = 0;
  m_uiBitsWritten   = 0;

  m_pulStreamPacket = m_apulStreamPacketBegin;
}

Void TComBitstream::destroy()
{
  delete [] m_apulStreamPacketBegin;     m_apulStreamPacketBegin = NULL;
}

// ====================================================================================================================
// Public member functions
// ====================================================================================================================

Void TComBitstream::write   ( UInt uiBits, UInt uiNumberOfBits )
{
  assert( m_uiBufSize > 0 );
  assert( uiNumberOfBits <= 32 );
  assert( ! ( (uiBits >> 1) >> (uiNumberOfBits - 1)) ); // because shift with 32 has no effect

  m_uiBitsWritten += uiNumberOfBits;

  if( (Int)uiNumberOfBits < m_iValidBits)  // one word
  {
    m_iValidBits -= uiNumberOfBits;

    m_ulCurrentBits |= uiBits << m_iValidBits;

    return;
  }

  UInt uiShift = uiNumberOfBits - m_iValidBits;

  // add the last bits
  m_ulCurrentBits |= uiBits >> uiShift;

  *m_pulStreamPacket++ = xSwap( m_ulCurrentBits );


  // note: there is a problem with left shift with 32
  m_iValidBits = 32 - uiShift;

  m_ulCurrentBits = uiBits << m_iValidBits;

  if( 0 == uiShift )
  {
    m_ulCurrentBits = 0;
  }
}

Void TComBitstream::writeAlignOne()
{
  write( ( 1 << (m_iValidBits & 0x7) ) - 1, m_iValidBits & 0x7 );
  return;
}

Void TComBitstream::writeAlignZero()
{
  write( 0, m_iValidBits & 0x7 );
  return;
}

Void  TComBitstream::flushBuffer()
{
  if (m_iValidBits == 0)
    return;

  *m_pulStreamPacket = xSwap( m_ulCurrentBits );

  m_uiBitsWritten = (m_uiBitsWritten+7)/8;

  m_uiBitsWritten *= 8;
}

Void TComBitstream::initParsing ( UInt uiNumBytes )
{
  m_ulCurrentBits     = 0xdeaddead;
  m_uiNextBits        = 0xdeaddead;
  m_uiBitsLeft        = 0;
  m_iValidBits        = 0;
  m_uiDWordsLeft      = 0;

  m_uiBitsLeft        = uiNumBytes << 3;

  m_uiDWordsLeft      = m_uiBitsLeft >> 5;
  m_iValidBits        = -32;

  xReadNextWord();
  xReadNextWord();
}

Void TComBitstream::read (UInt uiNumberOfBits, UInt& ruiBits)
{
  UInt ui_right_shift;

  // check the number_of_bits parameter matches the range
  assert( uiNumberOfBits <= 32 );

  if( uiNumberOfBits > m_uiBitsLeft )
  {
    assert (0);
  }

  m_uiBitsLeft -= uiNumberOfBits;
  m_iValidBits -= uiNumberOfBits;

  if( 0 <= m_iValidBits )
  {
    // calculate the number of bits to extract the desired number of bits
    ui_right_shift = 32 - uiNumberOfBits ;

    // mask out the value
    ruiBits  = m_ulCurrentBits >> ui_right_shift;

    //prepare for next access
    m_ulCurrentBits = m_ulCurrentBits << uiNumberOfBits;
  }
  else
  {
    // mask out the value in the current word
    ruiBits = m_ulCurrentBits;

    // calculate the number of bits to extract the desired number of bits
    ui_right_shift = m_iValidBits + uiNumberOfBits ;

    // mask out the value in the next word
    ruiBits |= m_uiNextBits >> ui_right_shift;

    ruiBits >>= 32 - uiNumberOfBits;

    m_uiNextBits <<=  -m_iValidBits;
  }

  // check the current word for being empty
  //-- baekeun.lee@samsung.com
  if ( 0 < m_iValidBits)
  {
    return ;
  }
  xReadNextWord();
}

Void TComBitstream::readAlignOne()
{
  UInt uiNumberOfBits = getBitsUntilByteAligned();

  // check the number_of_bits parameter matches the range
  assert (uiNumberOfBits <= 32);
  assert (uiNumberOfBits <= m_uiBitsLeft);

  // sub the desired number of bits
  m_uiBitsLeft -= uiNumberOfBits;
  m_iValidBits -= uiNumberOfBits;

  assert (m_uiBitsLeft%8 == 0);
  assert (m_iValidBits%8 == 0);

  // check the current word for beeing still valid
  if( 0 < m_iValidBits )
  {
    m_ulCurrentBits <<= uiNumberOfBits;
    return;
  }

  xReadNextWord();

  // shift to the right position
  m_ulCurrentBits <<= 32 - m_iValidBits;

  return;
}

// ====================================================================================================================
// Protected member functions
// ====================================================================================================================

__inline Void TComBitstream::xReadNextWord()
{
  m_ulCurrentBits = m_uiNextBits;
  m_iValidBits += 32;

  // chech if there are bytes left in the packet
  if( m_uiDWordsLeft )
  {
    // read 32 bit from the packet
    m_uiNextBits = xSwap( *m_pulStreamPacket++ );
    m_uiDWordsLeft--;
  }
  else
  {
    Int iBytesLeft  = ((Int)m_uiBitsLeft - m_iValidBits+7) >> 3;
    UChar* puc      = (UChar*) m_pulStreamPacket;
    m_uiNextBits  = 0;

    if( iBytesLeft > 0)
    {
      for( Int iByte = 0; iByte < iBytesLeft; iByte++ )
      {
        m_uiNextBits <<= 8;
        m_uiNextBits += puc[iByte];
      }
      m_uiNextBits <<= (4-iBytesLeft)<<3;
    }
  }
}