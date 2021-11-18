#ifndef BLBENCH_MODULE_SKIA_H
#define BLBENCH_MODULE_SKIA_H

#include "GrDirectContext.h"
#include "gl/GrGLInterface.h"
#include "SkData.h"
#include "SkImage.h"
#include "SkStream.h"
#include "SkSurface.h"

#include "./module.h"
#include <vector>

namespace blbench
{
	struct SkiaUtil
	{
		static uint32_t toSkiaFormat(uint32_t format);
		static uint32_t toSkiaOperator(uint32_t compOp);
	};

	class SkiaModule : public BenchModule
	{
	private:
		SkCanvas *m_canvas;
		sk_sp<GrDirectContext> m_context;
		sk_sp<SkSurface> m_surface;
		std::vector<SkBitmap> m_skiaSprites;

	public:
		explicit SkiaModule();
		virtual ~SkiaModule();
		virtual bool supportsCompOp(uint32_t compOp) const;
		virtual bool supportsStyle(uint32_t style) const;
		virtual void onBeforeRun();
		virtual void onAfterRun();
		virtual void onDoRectAligned(bool stroke);
		virtual void onDoRectSmooth(bool stroke);
		virtual void onDoRectRotated(bool stroke);
		virtual void onDoRoundSmooth(bool stroke);
		virtual void onDoRoundRotated(bool stroke);
		virtual void onDoPolygon(uint32_t mode, uint32_t complexity);
		virtual void onDoShape(bool stroke, const BLPoint *pts, size_t count);
	};

} // {blbench}

#endif // BLBENCH_MODULE_SKIA_H