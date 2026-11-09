package com.aurele.syncflorapotapp.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.Login
import androidx.compose.material.icons.filled.Email
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.PersonAdd
import androidx.compose.material.icons.filled.Visibility
import androidx.compose.material.icons.filled.VisibilityOff
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.text.input.VisualTransformation
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.aurele.syncflorapotapp.ui.components.AppBackground
import com.aurele.syncflorapotapp.ui.theme.AppColors
import com.aurele.syncflorapotapp.ui.theme.AppRadius
import com.aurele.syncflorapotapp.ui.theme.AppSpacing

@Composable
fun LoginScreen(
    isLoading: Boolean = false,
    errorMessage: String? = null,
    onLoginClick: (String, String) -> Unit = { _, _ -> },
    onRegisterClick: (String, String) -> Unit = { _, _ -> },
    onClearError: () -> Unit = {}
) {
    var email by remember { mutableStateOf("") }
    var password by remember { mutableStateOf("") }
    var showPassword by remember { mutableStateOf(false) }
    var isRegisterMode by remember { mutableStateOf(false) }
    var localError by remember { mutableStateOf<String?>(null) }

    val displayedError = errorMessage ?: localError

    AppBackground {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .verticalScroll(rememberScrollState())
                .padding(horizontal = AppSpacing.lg)
                .padding(top = AppSpacing.huge, bottom = AppSpacing.xl),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Spacer(modifier = Modifier.height(AppSpacing.xxxl))

            Text(
                text = "SyncFloraPot",
                fontSize = 36.sp,
                fontWeight = FontWeight.Bold,
                color = AppColors.TextPrimary
            )
            Text(
                text = "Smart Plant Monitoring & Care",
                fontSize = 14.sp,
                color = AppColors.TextSecondary,
                textAlign = TextAlign.Center,
                modifier = Modifier.padding(top = AppSpacing.xs)
            )

            Spacer(modifier = Modifier.height(AppSpacing.xxxl))

            displayedError?.let { error ->
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clip(RoundedCornerShape(AppRadius.lg))
                        .background(AppColors.Error.copy(alpha = 0.2f))
                        .border(
                            width = 1.dp,
                            color = AppColors.Error.copy(alpha = 0.4f),
                            shape = RoundedCornerShape(AppRadius.lg)
                        )
                        .padding(AppSpacing.md),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Box(
                        modifier = Modifier
                            .width(4.dp)
                            .height(24.dp)
                            .background(AppColors.Error, RoundedCornerShape(2.dp))
                    )
                    Spacer(modifier = Modifier.width(AppSpacing.sm))
                    Text(
                        text = error,
                        color = AppColors.Error,
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold
                    )
                }
                Spacer(modifier = Modifier.height(AppSpacing.md))
            }

            Column(
                modifier = Modifier.fillMaxWidth(),
                verticalArrangement = Arrangement.spacedBy(AppSpacing.sm)
            ) {
                Text(
                    text = "Email",
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.TextPrimary,
                    modifier = Modifier.padding(horizontal = AppSpacing.sm)
                )
                GlassInputField(
                    value = email,
                    onValueChange = {
                        email = it
                        localError = null
                        onClearError()
                    },
                    placeholder = "you@example.com",
                    leadingIcon = Icons.Default.Email,
                    keyboardType = KeyboardType.Email,
                    enabled = !isLoading
                )
            }

            Spacer(modifier = Modifier.height(AppSpacing.lg))

            Column(
                modifier = Modifier.fillMaxWidth(),
                verticalArrangement = Arrangement.spacedBy(AppSpacing.sm)
            ) {
                Text(
                    text = "Password",
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.TextPrimary,
                    modifier = Modifier.padding(horizontal = AppSpacing.sm)
                )
                GlassInputField(
                    value = password,
                    onValueChange = {
                        password = it
                        localError = null
                        onClearError()
                    },
                    placeholder = "••••••••",
                    leadingIcon = Icons.Default.Lock,
                    keyboardType = KeyboardType.Password,
                    visualTransformation = if (showPassword) VisualTransformation.None else PasswordVisualTransformation(),
                    trailingIcon = {
                        Icon(
                            imageVector = if (showPassword) Icons.Default.VisibilityOff else Icons.Default.Visibility,
                            contentDescription = "Toggle password",
                            tint = AppColors.TextSecondary,
                            modifier = Modifier
                                .size(20.dp)
                                .clickable { showPassword = !showPassword }
                        )
                    },
                    enabled = !isLoading
                )
            }

            Spacer(modifier = Modifier.height(AppSpacing.xl))

            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(52.dp)
                    .clip(RoundedCornerShape(AppRadius.lg))
                    .background(if (isLoading) AppColors.Primary.copy(alpha = 0.6f) else AppColors.Primary)
                    .clickable(enabled = !isLoading) {
                        if (email.trim().isEmpty() || password.trim().isEmpty()) {
                            localError = "Please enter email and password"
                        } else {
                            localError = null
                            if (isRegisterMode) {
                                onRegisterClick(email.trim(), password.trim())
                            } else {
                                onLoginClick(email.trim(), password.trim())
                            }
                        }
                    },
                contentAlignment = Alignment.Center
            ) {
                if (isLoading) {
                    CircularProgressIndicator(
                        color = AppColors.TextPrimary,
                        modifier = Modifier.size(24.dp),
                        strokeWidth = 2.dp
                    )
                } else {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(AppSpacing.md)
                    ) {
                        Icon(
                            imageVector = if (isRegisterMode) Icons.Default.PersonAdd else Icons.AutoMirrored.Filled.Login,
                            contentDescription = null,
                            tint = AppColors.TextPrimary,
                            modifier = Modifier.size(20.dp)
                        )
                        Text(
                            text = if (isRegisterMode) "Sign Up" else "Login",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = AppColors.TextPrimary
                        )
                    }
                }
            }

            Spacer(modifier = Modifier.height(AppSpacing.xxxl))

            Row(
                horizontalArrangement = Arrangement.Center,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = if (isRegisterMode) "Already have an account? " else "Don't have an account? ",
                    fontSize = 12.sp,
                    color = AppColors.TextSecondary
                )
                Text(
                    text = if (isRegisterMode) "Login here" else "Sign up here",
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.Primary,
                    modifier = Modifier.clickable {
                        isRegisterMode = !isRegisterMode
                        localError = null
                        onClearError()
                    }
                )
            }
        }
    }
}

@Composable
private fun GlassInputField(
    value: String,
    onValueChange: (String) -> Unit,
    placeholder: String,
    leadingIcon: ImageVector,
    keyboardType: KeyboardType = KeyboardType.Text,
    visualTransformation: VisualTransformation = VisualTransformation.None,
    trailingIcon: (@Composable () -> Unit)? = null,
    enabled: Boolean = true
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .height(48.dp)
            .clip(RoundedCornerShape(AppRadius.lg))
            .background(AppColors.GlassBgStrong)
            .border(1.dp, AppColors.GlassBorderFocus, RoundedCornerShape(AppRadius.lg))
            .padding(horizontal = AppSpacing.md),
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.spacedBy(AppSpacing.sm)
    ) {
        Icon(
            imageVector = leadingIcon,
            contentDescription = null,
            tint = AppColors.TextSecondary,
            modifier = Modifier.size(20.dp)
        )

        Box(modifier = Modifier.weight(1f)) {
            if (value.isEmpty()) {
                Text(
                    text = placeholder,
                    color = AppColors.TextTertiary,
                    fontSize = 14.sp
                )
            }
            BasicTextField(
                value = value,
                onValueChange = onValueChange,
                enabled = enabled,
                textStyle = TextStyle(
                    color = AppColors.TextPrimary,
                    fontSize = 14.sp,
                    fontWeight = FontWeight.Medium
                ),
                cursorBrush = SolidColor(AppColors.Primary),
                keyboardOptions = KeyboardOptions(keyboardType = keyboardType),
                visualTransformation = visualTransformation,
                singleLine = true,
                modifier = Modifier.fillMaxWidth()
            )
        }

        trailingIcon?.invoke()
    }
}