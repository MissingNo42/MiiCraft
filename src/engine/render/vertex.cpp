//
// Created by romai on 02/03/2025.
//

#include "engine/render/vertex.h"
#include "engine/render/renderer.h"


void ColoredVertex::mapVertexFormat() {
	// setup the GX vertex attribute table
	GX_ClearVtxDesc();
	GX_InvVtxCache();

	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

	Renderer::textureMap(TextureMap::None);
	Renderer::colorMap(true);
}


void ColoredVertex::mapVertexShader() {
	GX_SetNumTexGens(0);
	GX_SetNumTevStages(1);

	/// TEV: (D +/- (C - 1) * A + C * B + Bias) * Scale
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
}


void Vertex::mapVertexFormat() {
	// setup the GX vertex attribute table
	GX_ClearVtxDesc();
	GX_InvVtxCache();

	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

	Renderer::textureMap(TextureMap::Texture0);
	Renderer::colorMap(true);
}


void Vertex::mapVertexShader() {
	Renderer::textureLinearFilter(false);

	GX_SetNumTexGens(1);
	GX_SetNumTevStages(1);

	/// TEV: (D +/- (C - 1) * A + C * B + Bias) * Scale
	// GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
	// GX_SetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);  // GX_CC_KONST
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);  // GX_CA_KONST
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
}


void AdvancedVertex::mapVertexFormat() {
	// setup the GX vertex attribute table
	GX_ClearVtxDesc();
	GX_InvVtxCache();

	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

	Renderer::textureMap(TextureMap::Texture3);
	Renderer::colorMap(false);
}


void AdvancedVertex::mapVertexShader() {
	Renderer::textureLinearFilter(true);

	GX_SetNumTexGens(4);
	GX_SetNumTevStages(7);

	/// TEV: (D +/- (C - 1) * A + C * B + Bias) * Scale

	/*
		 * TS0: night multiplier                 :-----
		 *              				              |
		 * TS2: blend                            :    |------
		 *                                            |     |
		 * TS1: blue sky multiplier              :-----     |
		 *                                                  |
		 * TS4: blend                            :          |------
		 *                                                  |     |
		 * TS3: sunrise/sunset sky multiplier    :-----------     |
		 *                                                        |
		 * TS6: blend                            :                |--------> final color
		 *                                                        |
		 * TS5: sunrise/sunset effect multiplier :-----------------
		 */

		#define BlendStage(TEV, OUT) \
		GX_SetTevColorIn(TEV, GX_CC_C0, GX_CC_C1, GX_CC_A1, GX_CC_ZERO); \
		GX_SetTevAlphaIn(TEV, GX_CA_A0, GX_CA_ZERO, GX_CA_A1, GX_CA_A1); \
		GX_SetTevColorOp(TEV, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, OUT); \
		GX_SetTevAlphaOp(TEV, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, OUT); \
		GX_SetTevOrder(TEV, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLORNULL);

		/// multiply the night sky to REG0 (this set the texture opacity)
		GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
		GX_SetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);

		// multiply the blue sky to REG1 (this set the texture opacity)
		GX_SetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
		GX_SetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
		GX_SetTevColorIn(GX_TEVSTAGE1,GX_CC_ZERO,GX_CC_TEXC,GX_CC_KONST, GX_CC_ZERO);
		GX_SetTevAlphaIn(GX_TEVSTAGE1,GX_CA_ZERO,GX_CA_TEXA,GX_CA_KONST,GX_CA_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR0A0);

		// blend the blue sky on the night sky to REG0
		BlendStage(GX_TEVSTAGE2, GX_TEVREG0);

		// multiply the sunset/sunrise sky to REG1 (this set the texture opacity)
		GX_SetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
		GX_SetTevKAlphaSel(GX_TEVSTAGE3, GX_TEV_KASEL_K2_A);
		GX_SetTevColorIn(GX_TEVSTAGE3,GX_CC_ZERO,GX_CC_TEXC,GX_CC_KONST, GX_CC_ZERO);
		GX_SetTevAlphaIn(GX_TEVSTAGE3,GX_CA_ZERO,GX_CA_TEXA,GX_CA_KONST,GX_CA_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GX_SetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD2, GX_TEXMAP0, GX_COLOR0A0);

		// blend to REG0
		BlendStage(GX_TEVSTAGE4, GX_TEVREG0);

		// multiply the sunset/sunrise effect to REG1 (this set the texture opacity)
		GX_SetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K3);
		GX_SetTevKAlphaSel(GX_TEVSTAGE5, GX_TEV_KASEL_K3_A);
		GX_SetTevColorIn(GX_TEVSTAGE5,GX_CC_ZERO,GX_CC_TEXC,GX_CC_KONST, GX_CC_ZERO);
		GX_SetTevAlphaIn(GX_TEVSTAGE5,GX_CA_ZERO,GX_CA_TEXA,GX_CA_KONST,GX_CA_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GX_SetTevAlphaOp(GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GX_SetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD3, GX_TEXMAP0, GX_COLOR0A0);

		// blend to output REG3
		BlendStage(GX_TEVSTAGE6, GX_TEVPREV);
}
