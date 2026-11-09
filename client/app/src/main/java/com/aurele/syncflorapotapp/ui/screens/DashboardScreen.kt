package com.aurele.syncflorapotapp.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ExitToApp
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.LocationOn
import androidx.compose.material.icons.filled.Spa
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import coil.compose.AsyncImage
import com.aurele.syncflorapotapp.data.api.PotDto
import com.aurele.syncflorapotapp.data.local.PlantCatalog
import com.aurele.syncflorapotapp.ui.components.AppBackground
import com.aurele.syncflorapotapp.ui.theme.AppColors
import com.aurele.syncflorapotapp.ui.theme.AppRadius
import com.aurele.syncflorapotapp.ui.theme.AppSpacing

data class PotUiModel(
    val id: String,
    val name: String,
    val species: String?,
    val location: String?,
    val connected: Boolean
)

@Composable
fun DashboardScreen(
    pots: List<PotDto>,
    userEmail: String,
    isLoading: Boolean,
    onLogoutClick: () -> Unit,
    onPotClick: (String, String) -> Unit,
    onAddPotClick: () -> Unit
) {
    AppBackground {
        Box(modifier = Modifier.fillMaxSize()) {
            LazyColumn(
                contentPadding = PaddingValues(
                    start = AppSpacing.lg,
                    top = AppSpacing.huge,
                    end = AppSpacing.lg,
                    bottom = 90.dp
                ),
                verticalArrangement = Arrangement.spacedBy(AppSpacing.md),
                modifier = Modifier.fillMaxSize()
            ) {
                item {
                    TopHeaderBar(
                        userEmail = userEmail,
                        onLogoutClick = onLogoutClick
                    )
                }

                item {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(top = AppSpacing.xs, bottom = 2.dp),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = "My Devices",
                            fontSize = 18.sp,
                            fontWeight = FontWeight.Bold,
                            color = AppColors.TextPrimary
                        )

                        Text(
                            text = "${pots.size} pot(s)",
                            fontSize = 12.sp,
                            color = AppColors.TextSecondary
                        )
                    }
                }

                if (pots.isEmpty() && !isLoading) {
                    item {
                        EmptyState(onAddPotClick = onAddPotClick)
                    }
                } else {
                    items(pots, key = { it.id }) { pot ->
                        PotEquipmentCard(
                            pot = PotUiModel(
                                id = pot.id,
                                name = pot.name,
                                species = pot.species,
                                location = pot.location,
                                connected = pot.connected
                            ),
                            onClick = { onPotClick(pot.id, pot.name) }
                        )
                    }
                }
            }

            AddPotFloatingButton(
                onClick = onAddPotClick,
                modifier = Modifier
                    .align(Alignment.BottomEnd)
                    .padding(AppSpacing.lg)
            )
        }
    }
}

@Composable
private fun TopHeaderBar(
    userEmail: String,
    onLogoutClick: () -> Unit
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(bottom = AppSpacing.md),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Row(
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(AppSpacing.md)
        ) {
            Box(
                modifier = Modifier
                    .size(44.dp)
                    .clip(CircleShape)
                    .background(AppColors.GlassBgStrong)
                    .border(1.dp, AppColors.GlassBorder, CircleShape),
                contentAlignment = Alignment.Center
            ) {
                Text(
                    text = userEmail.firstOrNull()?.uppercase() ?: "S",
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.Primary
                )
            }

            Column {
                Text(
                    text = "Hello 👋",
                    fontSize = 20.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.TextPrimary
                )
                if (userEmail.isNotBlank()) {
                    Text(
                        text = userEmail,
                        fontSize = 12.sp,
                        color = AppColors.TextSecondary
                    )
                }
            }
        }

        Box(
            modifier = Modifier
                .size(40.dp)
                .clip(CircleShape)
                .background(AppColors.GlassBgStrong)
                .border(1.dp, AppColors.GlassBorder, CircleShape)
                .clickable { onLogoutClick() },
            contentAlignment = Alignment.Center
        ) {
            Icon(
                imageVector = Icons.AutoMirrored.Filled.ExitToApp,
                contentDescription = "Logout",
                tint = AppColors.TextPrimary,
                modifier = Modifier.size(18.dp)
            )
        }
    }
}

@Composable
private fun PotEquipmentCard(
    pot: PotUiModel,
    onClick: () -> Unit
) {
    val matchedPlant = remember(pot.species) { com.aurele.syncflorapotapp.data.repository.PlantRepository.findPlant(pot.species) }

    Box(
        modifier = Modifier
            .fillMaxWidth()
            .clip(RoundedCornerShape(AppRadius.xl))
            .background(AppColors.GlassBg)
            .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.xl))
            .clickable { onClick() }
            .padding(AppSpacing.lg)
    ) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(AppSpacing.md),
                modifier = Modifier.weight(1f)
            ) {
                Box(
                    modifier = Modifier
                        .size(52.dp)
                        .clip(RoundedCornerShape(AppRadius.lg))
                        .background(AppColors.Primary.copy(alpha = 0.15f))
                        .border(1.dp, AppColors.Primary.copy(alpha = 0.3f), RoundedCornerShape(AppRadius.lg)),
                    contentAlignment = Alignment.Center
                ) {
                    if (matchedPlant != null) {
                        AsyncImage(
                            model = matchedPlant.imageUrl,
                            contentDescription = null,
                            contentScale = ContentScale.Crop,
                            modifier = Modifier.fillMaxSize()
                        )
                    } else {
                        Icon(
                            imageVector = Icons.Default.Spa,
                            contentDescription = null,
                            tint = AppColors.Primary,
                            modifier = Modifier.size(24.dp)
                        )
                    }
                }

                Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                    Text(
                        text = pot.name,
                        fontSize = 17.sp,
                        fontWeight = FontWeight.Bold,
                        color = AppColors.TextPrimary
                    )

                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(AppSpacing.xs)
                    ) {
                        pot.species?.takeIf { it.isNotBlank() }?.let { sp ->
                            Text(
                                text = sp,
                                fontSize = 12.sp,
                                color = AppColors.Accent
                            )
                        }

                        if (!pot.species.isNullOrBlank() && !pot.location.isNullOrBlank()) {
                            Text(
                                text = "•",
                                fontSize = 10.sp,
                                color = AppColors.TextMuted
                            )
                        }

                        pot.location?.takeIf { it.isNotBlank() }?.let { loc ->
                            Row(
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.spacedBy(2.dp)
                            ) {
                                Icon(
                                    imageVector = Icons.Default.LocationOn,
                                    contentDescription = null,
                                    tint = AppColors.TextSecondary,
                                    modifier = Modifier.size(12.dp)
                                )
                                Text(
                                    text = loc,
                                    fontSize = 12.sp,
                                    color = AppColors.TextSecondary
                                )
                            }
                        }
                    }
                }
            }

            Row(
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(AppSpacing.sm)
            ) {
                Box(
                    modifier = Modifier
                        .clip(CircleShape)
                        .background(if (pot.connected) AppColors.Success.copy(alpha = 0.15f) else AppColors.Error.copy(alpha = 0.15f))
                        .border(
                            1.dp,
                            if (pot.connected) AppColors.Success.copy(alpha = 0.5f) else AppColors.Error.copy(alpha = 0.5f),
                            CircleShape
                        )
                        .padding(horizontal = 8.dp, vertical = 4.dp)
                ) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(4.dp)
                    ) {
                        Box(
                            modifier = Modifier
                                .size(6.dp)
                                .clip(CircleShape)
                                .background(if (pot.connected) AppColors.Success else AppColors.Error)
                        )
                        Text(
                            text = if (pot.connected) "Online" else "Offline",
                            fontSize = 11.sp,
                            fontWeight = FontWeight.Medium,
                            color = if (pot.connected) AppColors.Success else AppColors.Error
                        )
                    }
                }

                Icon(
                    imageVector = Icons.AutoMirrored.Filled.KeyboardArrowRight,
                    contentDescription = "View",
                    tint = AppColors.TextSecondary,
                    modifier = Modifier.size(20.dp)
                )
            }
        }
    }
}

@Composable
private fun EmptyState(onAddPotClick: () -> Unit) {
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .clip(RoundedCornerShape(AppRadius.xl))
            .background(AppColors.GlassBg)
            .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.xl))
            .clickable { onAddPotClick() }
            .padding(AppSpacing.xxxl),
        contentAlignment = Alignment.Center
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.spacedBy(AppSpacing.sm)
        ) {
            Icon(
                imageVector = Icons.Default.Spa,
                contentDescription = null,
                tint = AppColors.Primary,
                modifier = Modifier.size(54.dp)
            )
            Text(
                text = "No pots configured",
                fontSize = 18.sp,
                fontWeight = FontWeight.Bold,
                color = AppColors.TextPrimary
            )
            Text(
                text = "Tap to add your first pot",
                fontSize = 13.sp,
                color = AppColors.TextSecondary
            )
        }
    }
}

@Composable
private fun AddPotFloatingButton(onClick: () -> Unit, modifier: Modifier = Modifier) {
    Box(
        modifier = modifier
            .size(56.dp)
            .clip(CircleShape)
            .background(AppColors.Primary)
            .border(1.dp, AppColors.GlassBorder, CircleShape)
            .clickable { onClick() },
        contentAlignment = Alignment.Center
    ) {
        Icon(
            imageVector = Icons.Default.Add,
            contentDescription = "Add pot",
            tint = AppColors.TextPrimary,
            modifier = Modifier.size(26.dp)
        )
    }
}