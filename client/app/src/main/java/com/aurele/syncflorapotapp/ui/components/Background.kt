package com.aurele.syncflorapotapp.ui.components

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxScope
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import com.aurele.syncflorapotapp.ui.theme.AppColors
import kotlin.math.sqrt

@Composable
fun AppBackground(content: @Composable BoxScope.() -> Unit) {
    Box(modifier = Modifier.fillMaxSize()) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            drawRect(color = AppColors.Background)

            val baseRadius = sqrt(size.width * size.height)

            drawRect(
                brush = Brush.radialGradient(
                    colors = listOf(AppColors.GradientCyan.copy(alpha = 0.24f), Color.Transparent),
                    center = Offset(0f, 0f),
                    radius = baseRadius * 1.3f
                )
            )

            drawRect(
                brush = Brush.radialGradient(
                    colors = listOf(AppColors.GradientPurple.copy(alpha = 0.21f), Color.Transparent),
                    center = Offset(size.width, size.height),
                    radius = baseRadius * 1.3f
                )
            )

            drawRect(
                brush = Brush.radialGradient(
                    colors = listOf(AppColors.GradientEmerald.copy(alpha = 0.15f), Color.Transparent),
                    center = Offset(size.width / 2f, 0f),
                    radius = baseRadius * 0.8f
                )
            )

            drawRect(
                brush = Brush.radialGradient(
                    colors = listOf(AppColors.GradientGold.copy(alpha = 0.21f), Color.Transparent),
                    center = Offset(0f, size.height),
                    radius = baseRadius * 1.2f
                )
            )
        }
        content()
    }
}