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

/** \file			TDecCAVLC.h
    \brief		CAVLC decoder class (header)
*/

#ifndef __TDECCAVLC__
#define __TDECCAVLC__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDecEntropy.h"

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// CAVLC decoder class
class TDecCavlc : public TDecEntropyIf
{
public:
  TDecCavlc();
  virtual ~TDecCavlc();

protected:
  Void  xReadCode						( UInt uiLength, UInt& ruiCode );
  Void  xReadUvlc						( UInt& ruiVal  );
  Void  xReadSvlc						( Int&  riVal   );
  Void  xReadFlag						( UInt& ruiCode );
	Void  xReadEpExGolomb     ( UInt& ruiSymbol, UInt uiCount );
	Void  xReadExGolombLevel  ( UInt& ruiSymbol );
	Void  xReadUnaryMaxSymbol ( UInt& ruiSymbol, UInt uiMaxSymbol );

	UInt  xGetBit             ();
  Int   xReadVlc            ( Int n );
  Void	xParseCoeff4x4			( TCoeff* scoeff, Int iTableNumber );
  Void	xParseCoeff8x8			( TCoeff* scoeff, Int iTableNumber );

private:
  TComBitstream*						m_pcBitstream;
  UInt											m_uiCoeffCost;
  Bool											m_bRunLengthCoding;
  UInt											m_uiRun;
  Bool											m_bAlfCtrl;
  UInt											m_uiMaxAlfCtrlDepth;
  UInt											m_uiLPTableD4[3][32];
  UInt											m_uiLPTableD8[10][128];
  UInt											m_uiLastPosVlcIndex[10];

public:
  Void  resetEntropy				( TComSlice* pcSlice	);
  Void  setBitstream				( TComBitstream* p		)			{ m_pcBitstream = p; }
  Void  preloadSbac					()													{ assert (0); }
  Void  setAlfCtrl					( Bool bAlfCtrl )						{ m_bAlfCtrl = bAlfCtrl; }
  Void  setMaxAlfCtrlDepth	( UInt uiMaxAlfCtrlDepth )	{ m_uiMaxAlfCtrlDepth = uiMaxAlfCtrlDepth; }

  Void	parseAlfFlag				( UInt& ruiVal );
  Void	parseAlfUvlc				( UInt& ruiVal );
  Void	parseAlfSvlc				( Int&  riVal  );

	Void  parseSPS						( TComSPS* pcSPS );
  Void  parseSliceHeader    ( TComSlice*& rpcSlice );
	Void  parseTerminatingBit ( UInt& ruiBit );

  Void	parseMVPIdx					( TComDataCU* pcCU, Int& riMVPIdx, Int iMVPNum, UInt uiAbsPartIdx, UInt uiDepth, RefPicList eRefList );
  Void	parseSkipFlag				( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parseSplitFlag				( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parsePartSize				( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parsePredMode				( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );

  Void	parseIntraDirLuma		( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );

  Void	parseIntraDirLumaAdi( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );

  Void	parseIntraDirChroma	( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );

  Void	parseInterDir				( TComDataCU* pcCU, UInt& ruiInterDir, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parseRefFrmIdx			( TComDataCU* pcCU, Int& riRefFrmIdx,	 UInt uiAbsPartIdx, UInt uiDepth, RefPicList eRefList );
  Void	parseMvd						( TComDataCU* pcCU, UInt uiAbsPartAddr,UInt uiPartIdx,		UInt uiDepth, RefPicList eRefList );

  Void	parseTransformIdx		( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parseDeltaQP				( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parseCbf						( TComDataCU* pcCU, UInt uiAbsPartIdx, TextType eType, UInt uiTrDepth, UInt uiDepth );
  Void	parseCoeffNxN				( TComDataCU* pcCU, TCoeff* pcCoef, UInt uiAbsPartIdx, UInt uiWidth, UInt uiHeight, UInt uiDepth, TextType eTType );

  Void	parseROTindex				( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parseCIPflag				( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );
  Void	parseAlfCtrlDepth		( UInt& ruiAlfCtrlDepth );
  Void	parseAlfCtrlFlag		( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth );

  //  for debugging
  UInt  getRange()  { assert(0); return 0;  }
  UInt  getValue()  { assert(0); return 0;  }
};

#endif
