/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveMaterialBatcher.cpp
 *  PURPOSE:
 *
 *
 *****************************************************************************/
#include <StdInc.h>
#include "CPrimitiveMaterialBatcher.h"
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::CPrimitiveMaterialBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveMaterialBatcher::CPrimitiveMaterialBatcher(bool m_bZTest, CGraphics* graphics)
{
    m_bZTest = m_bZTest;
    m_pGraphics = graphics;
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::~CPrimitiveMaterialBatcher
//
//
//
////////////////////////////////////////////////////////////////
CPrimitiveMaterialBatcher::~CPrimitiveMaterialBatcher(void)
{
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY)
{
    m_pDevice = pDevice;
    // Cache matrices
    UpdateMatrices(fViewportSizeX, fViewportSizeY);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::OnRenderTargetChange
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY)
{
    // Flush dx draws
    Flush();
    // Make new projection transform
    UpdateMatrices(uiNewViewportSizeX, uiNewViewportSizeY);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::UpdateMatrices
//
//
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::UpdateMatrices(float fViewportSizeX, float fViewportSizeY)
{
    m_fViewportSizeX = fViewportSizeX;
    m_fViewportSizeY = fViewportSizeY;
    D3DXMatrixIdentity(&m_MatView);
    D3DXMatrixIdentity(&m_MatProjection);
    // Make projection 3D friendly, so shaders can alter the z coord for fancy effects
    float fFarPlane = 10000;
    float fNearPlane = 100;
    float Q = fFarPlane / (fFarPlane - fNearPlane);
    float fAdjustZFactor = 1000.f;
    float rcpSizeX = 2.0f / m_fViewportSizeX;
    float rcpSizeY = -2.0f / m_fViewportSizeY;
    rcpSizeX *= fAdjustZFactor;
    rcpSizeY *= fAdjustZFactor;
    m_MatProjection.m[0][0] = rcpSizeX;
    m_MatProjection.m[1][1] = rcpSizeY;
    m_MatProjection.m[2][2] = Q;
    m_MatProjection.m[2][3] = 1;
    m_MatProjection.m[3][0] = (-m_fViewportSizeX / 2.0f - 0.5f) * rcpSizeX;
    m_MatProjection.m[3][1] = (-m_fViewportSizeY / 2.0f - 0.5f) * rcpSizeY;
    m_MatProjection.m[3][2] = -Q * fNearPlane;
    m_MatProjection.m[3][3] = 0;
    m_MatView.m[3][2] = fAdjustZFactor;
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::Flush
//
// Send all buffered vertices to D3D
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::Flush(void)
{
    if (m_primitiveList.empty())
        return;

    // Save render states
    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);
    // Set transformations
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
    m_pDevice->SetTransform(D3DTS_VIEW, &m_MatView);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &m_MatProjection);

    // Set vertex FVF
    m_pDevice->SetFVF(PrimitiveMaterialVertice::FNV);

    // Set states
    m_pDevice->SetRenderState(D3DRS_ZENABLE, m_bZTest ? D3DZB_TRUE : D3DZB_FALSE);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // Draw
    m_pDevice->SetTexture(0, nullptr);
    // Cache last used material, so we don't set directx parameters needlessly
    CMaterialItem* pLastMaterial = nullptr;

    for (int i = 0; i < m_primitiveList.size(); i++)
    {
        sDrawQueuePrimitiveMaterial primitive = m_primitiveList[i];
        // uint PrimitiveCount = m_triangleList.size () / 3;
        const void* pVertexStreamZeroData = &primitive.vertices[0];
        uint        uiVertexStreamZeroStride = sizeof(PrimitiveMaterialVertice);

        CMaterialItem* pMaterial = primitive.material;
        if (pMaterial != pLastMaterial)
        {
            // Set texture addressing mode
            m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, pMaterial->m_TextureAddress);
            m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, pMaterial->m_TextureAddress);

            if (pMaterial->m_TextureAddress == TADDRESS_BORDER)
                m_pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, pMaterial->m_uiBorderColor);
        }

        if (CTextureItem* pTextureItem = DynamicCast<CTextureItem>(pMaterial))
        {
            // Draw using texture
            if (pMaterial != pLastMaterial)
            {
                m_pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
            }

            DrawPrimitive(primitive.type, primitive.vertices.size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
        }
        else if (CShaderInstance* pShaderInstance = DynamicCast<CShaderInstance>(pMaterial))
        {
            // Draw using shader
            ID3DXEffect* pD3DEffect = pShaderInstance->m_pEffectWrap->m_pD3DEffect;

            if (pMaterial != pLastMaterial)
            {
                // Apply custom parameters
                pShaderInstance->ApplyShaderParameters();
                // Apply common parameters
                pShaderInstance->m_pEffectWrap->ApplyCommonHandles();
                // Apply mapped parameters
                pShaderInstance->m_pEffectWrap->ApplyMappedHandles();
            }

            // Do shader passes
            DWORD dwFlags = D3DXFX_DONOTSAVESHADERSTATE;
            uint  uiNumPasses = 0;
            pShaderInstance->m_pEffectWrap->Begin(&uiNumPasses, dwFlags, false);

            for (uint uiPass = 0; uiPass < uiNumPasses; uiPass++)
            {
                pD3DEffect->BeginPass(uiPass);
                DrawPrimitive(primitive.type, primitive.vertices.size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
                pD3DEffect->EndPass();
            }
            pShaderInstance->m_pEffectWrap->End();

            // If we didn't get the effect to save the shader state, clear some things here
            if (dwFlags & D3DXFX_DONOTSAVESHADERSTATE)
            {
                m_pDevice->SetVertexShader(NULL);
                m_pDevice->SetPixelShader(NULL);
            }
        }
        pLastMaterial = pMaterial;
        m_pGraphics->RemoveQueueRef(pMaterial);
    }

    // Clean up
    ClearQueue();
    // Restore render states
    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::DrawPrimitive
//
// Draws the primitives on render target
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::DrawPrimitive(D3DPRIMITIVETYPE eType, size_t iCollectionSize, const void* pDataAddr, size_t uiVertexStride)
{
    int iSize = 1;
    switch (eType)
    {
        case D3DPT_POINTLIST:
            iSize = iCollectionSize;
            break;
        case D3DPT_LINELIST:
            iSize = iCollectionSize / 2;
            break;
        case D3DPT_LINESTRIP:
            iSize = iCollectionSize - 1;
            break;
        case D3DPT_TRIANGLEFAN:
        case D3DPT_TRIANGLESTRIP:
            iSize = iCollectionSize - 2;
            break;
        case D3DPT_TRIANGLELIST:
            iSize = iCollectionSize / 3;
            break;
    }
    m_pDevice->DrawPrimitiveUP(eType, iSize, pDataAddr, uiVertexStride);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::ClearQueue
//
// Clears all primitives in current queue
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::ClearQueue()
{
    // Clean up
    size_t prevSize = m_primitiveList.size();
    m_primitiveList.clear();
    m_primitiveList.reserve(prevSize);
}
////////////////////////////////////////////////////////////////
//
// CPrimitiveMaterialBatcher::AddTriangle
//
// Add a new primitive to the list
//
////////////////////////////////////////////////////////////////
void CPrimitiveMaterialBatcher::AddPrimitive(sDrawQueuePrimitiveMaterial primitive)
{
    m_primitiveList.push_back(primitive);
}
