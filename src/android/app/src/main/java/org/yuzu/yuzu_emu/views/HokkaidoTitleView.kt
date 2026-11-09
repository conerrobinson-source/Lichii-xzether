// SPDX-FileCopyrightText: Copyright 2026 Hokkaido contributors
// SPDX-License-Identifier: GPL-3.0-or-later

package org.yuzu.yuzu_emu.views

import android.content.Context
import android.graphics.Canvas
import android.graphics.LinearGradient
import android.graphics.Paint
import android.graphics.Shader
import android.graphics.Typeface
import android.util.AttributeSet
import androidx.core.content.ContextCompat
import com.google.android.material.textview.MaterialTextView
import org.yuzu.yuzu_emu.R

/** The main screen's "Hokkaido" title, drawn like the logo: a light outline under an orange gradient. */
class HokkaidoTitleView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : MaterialTextView(context, attrs, defStyleAttr) {

    private val outlineColor = ContextCompat.getColor(context, R.color.hokkaido_title_outline)
    private val topColor = ContextCompat.getColor(context, R.color.hokkaido_title_top)
    private val bottomColor = ContextCompat.getColor(context, R.color.hokkaido_title_bottom)
    private val outlineWidth = resources.displayMetrics.density * 5f
    private var gradient: LinearGradient? = null
    private var gradientHeight = 0
    private var drawingLayers = false

    init {
        typeface = Typeface.create("sans-serif-black", Typeface.NORMAL)
        letterSpacing = 0.02f
        setShadowLayer(0f, 0f, 0f, 0)
        // Room for the outline around the glyphs.
        val pad = outlineWidth.toInt()
        setPadding(paddingLeft + pad, paddingTop + pad, paddingRight + pad, paddingBottom + pad)
    }

    // The two passes in onDraw change the text color; that must not schedule another draw.
    override fun invalidate() {
        if (!drawingLayers) {
            super.invalidate()
        }
    }

    override fun onDraw(canvas: Canvas) {
        if (gradient == null || gradientHeight != height) {
            gradientHeight = height
            gradient = LinearGradient(
                0f, paddingTop.toFloat(), 0f, (height - paddingBottom).toFloat(),
                topColor, bottomColor, Shader.TileMode.CLAMP
            )
        }
        drawingLayers = true
        val textPaint = paint
        textPaint.shader = null
        textPaint.style = Paint.Style.STROKE
        textPaint.strokeJoin = Paint.Join.ROUND
        textPaint.strokeWidth = outlineWidth
        setTextColor(outlineColor)
        super.onDraw(canvas)
        textPaint.style = Paint.Style.FILL
        textPaint.shader = gradient
        setTextColor(bottomColor)
        super.onDraw(canvas)
        textPaint.shader = null
        drawingLayers = false
    }
}
