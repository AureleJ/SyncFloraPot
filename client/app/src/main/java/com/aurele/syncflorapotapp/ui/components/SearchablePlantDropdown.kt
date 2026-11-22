package com.aurele.syncflorapotapp.ui.components

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Spa
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import coil.compose.AsyncImage
import com.aurele.syncflorapotapp.data.local.PlantCatalog
import com.aurele.syncflorapotapp.data.local.PlantSpeciesInfo
import com.aurele.syncflorapotapp.data.repository.PlantRepository
import com.aurele.syncflorapotapp.ui.theme.AppColors
import com.aurele.syncflorapotapp.ui.theme.AppRadius
import com.aurele.syncflorapotapp.ui.theme.AppSpacing

@Composable
fun SearchablePlantDropdown(
    value: String,
    onValueChange: (String) -> Unit,
    placeholder: String = "Search plant species",
    modifier: Modifier = Modifier
) {
    var expanded by remember { mutableStateOf(false) }
    var filteredPlants by remember { mutableStateOf<List<PlantSpeciesInfo>>(PlantCatalog.plants) }

    LaunchedEffect(value) {
        if (value.isNotBlank()) {
            filteredPlants = PlantRepository.searchPlants(value)
        } else {
            filteredPlants = PlantCatalog.plants
        }
    }

    Column(modifier = modifier.fillMaxWidth()) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(48.dp)
                .clip(RoundedCornerShape(AppRadius.lg))
                .background(AppColors.GlassBg)
                .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.lg))
                .padding(horizontal = AppSpacing.md),
            contentAlignment = Alignment.CenterStart
        ) {
            if (value.isEmpty()) {
                Text(
                    text = placeholder,
                    color = AppColors.TextTertiary,
                    fontSize = 14.sp
                )
            }
            BasicTextField(
                value = value,
                onValueChange = {
                    onValueChange(it)
                    expanded = true
                },
                textStyle = TextStyle(
                    color = AppColors.TextPrimary,
                    fontSize = 14.sp,
                    fontWeight = FontWeight.Medium
                ),
                cursorBrush = SolidColor(AppColors.Primary),
                keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Text),
                singleLine = true,
                modifier = Modifier.fillMaxWidth()
            )
        }

        AnimatedVisibility(
            visible = expanded && filteredPlants.isNotEmpty(),
            enter = fadeIn(),
            exit = fadeOut()
        ) {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 4.dp)
                    .clip(RoundedCornerShape(AppRadius.lg))
                    .background(AppColors.BackgroundSecondary)
                    .border(1.dp, AppColors.GlassBorderFocus, RoundedCornerShape(AppRadius.lg))
            ) {
                LazyColumn(
                    modifier = Modifier
                        .fillMaxWidth()
                        .heightIn(max = 220.dp)
                        .padding(vertical = 4.dp)
                ) {
                    items(filteredPlants) { plant ->
                        PlantSuggestionRow(
                            plant = plant,
                            onClick = {
                                onValueChange(plant.name)
                                expanded = false
                            }
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun PlantSuggestionRow(
    plant: PlantSpeciesInfo,
    onClick: () -> Unit
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .clickable { onClick() }
            .padding(horizontal = AppSpacing.md, vertical = AppSpacing.sm),
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.spacedBy(AppSpacing.md)
    ) {
        Box(
            modifier = Modifier
                .size(36.dp)
                .clip(CircleShape)
                .background(AppColors.Primary.copy(alpha = 0.15f))
                .border(1.dp, AppColors.Primary.copy(alpha = 0.3f), CircleShape),
            contentAlignment = Alignment.Center
        ) {
            if (plant.imageUrl.isNotEmpty()) {
                AsyncImage(
                    model = plant.imageUrl,
                    contentDescription = null,
                    contentScale = ContentScale.Crop,
                    modifier = Modifier.fillMaxSize()
                )
            } else {
                Icon(
                    imageVector = Icons.Default.Spa,
                    contentDescription = null,
                    tint = AppColors.Primary,
                    modifier = Modifier.size(18.dp)
                )
            }
        }

        Column(modifier = Modifier.weight(1f)) {
            Text(
                text = plant.name,
                fontSize = 14.sp,
                fontWeight = FontWeight.Bold,
                color = AppColors.TextPrimary
            )
            Text(
                text = "${plant.commonName} • ${plant.wateringTip}",
                fontSize = 11.sp,
                color = AppColors.TextSecondary
            )
        }
    }
}