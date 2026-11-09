package com.aurele.syncflorapotapp.ui.screens

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
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Analytics
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material.icons.filled.ElectricBolt
import androidx.compose.material.icons.filled.History
import androidx.compose.material.icons.filled.LocalDrink
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.filled.Spa
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material.icons.filled.WaterDrop
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import coil.compose.AsyncImage
import com.aurele.syncflorapotapp.data.api.MetricDto
import com.aurele.syncflorapotapp.data.local.PlantCatalog
import com.aurele.syncflorapotapp.ui.components.AppBackground
import com.aurele.syncflorapotapp.ui.theme.AppColors
import com.aurele.syncflorapotapp.ui.theme.AppRadius
import com.aurele.syncflorapotapp.ui.theme.AppSpacing
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

data class SensorThreshold(
    val min: Float,
    val max: Float,
    val warningMin: Float,
    val warningMax: Float
)

val SENSOR_THRESHOLDS = mapOf(
    "soil_moisture" to SensorThreshold(min = 20f, max = 80f, warningMin = 15f, warningMax = 90f),
    "light" to SensorThreshold(min = 10f, max = 100f, warningMin = 15f, warningMax = 100f),
    "water_level_state" to SensorThreshold(min = 20f, max = 100f, warningMin = 15f, warningMax = 100f),
    "pump_state" to SensorThreshold(min = 0f, max = 1f, warningMin = 0f, warningMax = 1f)
)

enum class HealthStatus {
    HEALTHY,
    WARNING,
    CRITICAL
}

data class MetricUiModel(
    val key: String,
    val title: String,
    val value: String,
    val numericValue: Float? = null,
    val statusText: String = ""
)

@Composable
fun PotDetailsScreen(
    potId: String = "",
    potName: String = "Monstera Living Room",
    species: String? = "Monstera Deliciosa",
    location: String? = "Living Room",
    isPotConnected: Boolean = false,
    metric: MetricDto? = null,
    history: List<MetricDto> = emptyList(),
    lastUpdateTimestamp: Long? = null,
    onBackClick: () -> Unit = {},
    onRefreshClick: () -> Unit = {},
    onWateringClick: () -> Unit = {},
    onUpdatePot: (name: String, species: String, location: String) -> Unit = { _, _, _ -> },
    onDeletePot: () -> Unit = {}
) {
    val isActuallyConnected = isPotConnected

    var isWateringLoading by remember { mutableStateOf(false) }
    var showSettingsDialog by remember { mutableStateOf(false) }
    var showDeleteConfirmDialog by remember { mutableStateOf(false) }
    val coroutineScope = rememberCoroutineScope()

    val soilMoisture = metric?.soilMoisture
    val lightLevel = metric?.light
    val waterLevel = metric?.waterLevel
    val pumpState = metric?.pumpState == true

    val metrics = listOf(
        MetricUiModel(
            key = "soil_moisture",
            title = "Soil Moisture",
            value = soilMoisture?.let { "${it.toInt()}%" } ?: "--",
            numericValue = soilMoisture,
            statusText = getSensorStatusText("soil_moisture", soilMoisture)
        ),
        MetricUiModel(
            key = "light",
            title = "Light Level",
            value = lightLevel?.let { "${it.toInt()}%" } ?: "--",
            numericValue = lightLevel,
            statusText = getSensorStatusText("light", lightLevel)
        ),
        MetricUiModel(
            key = "water_level_state",
            title = "Water Reservoir",
            value = waterLevel?.let { "${it.toInt()}%" } ?: "--",
            numericValue = waterLevel,
            statusText = getSensorStatusText("water_level_state", waterLevel)
        ),
        MetricUiModel(
            key = "pump_state",
            title = "Pump State",
            value = if (pumpState) "Active" else "Auto",
            numericValue = if (pumpState) 1f else 0f,
            statusText = if (pumpState) "Running" else "Standby"
        )
    )

    val globalHealth = computeGlobalHealth(metrics, isActuallyConnected)
    val isLowWater = isActuallyConnected && (waterLevel?.let { it <= 20f } ?: false)
    val matchedPlant = remember(species) { com.aurele.syncflorapotapp.data.repository.PlantRepository.findPlant(species) }

    AppBackground {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .verticalScroll(rememberScrollState())
                .padding(horizontal = AppSpacing.lg)
                .padding(top = AppSpacing.huge, bottom = AppSpacing.xl),
            verticalArrangement = Arrangement.spacedBy(AppSpacing.lg)
        ) {
            // 1. Navigation Top Bar
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Box(
                    modifier = Modifier
                        .size(40.dp)
                        .clip(CircleShape)
                        .background(AppColors.GlassBgStrong)
                        .border(1.dp, AppColors.GlassBorder, CircleShape)
                        .clickable { onBackClick() },
                    contentAlignment = Alignment.Center
                ) {
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                        contentDescription = "Back",
                        tint = AppColors.TextPrimary,
                        modifier = Modifier.size(20.dp)
                    )
                }

                // Status Badge
                Row(
                    modifier = Modifier
                        .clip(CircleShape)
                        .background(AppColors.GlassBgStrong)
                        .border(1.dp, AppColors.GlassBorder, CircleShape)
                        .padding(horizontal = AppSpacing.md, vertical = 6.dp),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    Box(
                        modifier = Modifier
                            .size(8.dp)
                            .clip(CircleShape)
                            .background(if (isActuallyConnected) AppColors.Success else AppColors.Error)
                    )
                    Text(
                        text = if (isActuallyConnected) "Online" else "Offline",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Medium,
                        color = AppColors.TextSecondary
                    )
                }

                Box(
                    modifier = Modifier
                        .size(40.dp)
                        .clip(CircleShape)
                        .background(AppColors.GlassBgStrong)
                        .border(1.dp, AppColors.GlassBorder, CircleShape)
                        .clickable { showSettingsDialog = true },
                    contentAlignment = Alignment.Center
                ) {
                    Icon(
                        imageVector = Icons.Default.Settings,
                        contentDescription = "Settings",
                        tint = AppColors.TextPrimary,
                        modifier = Modifier.size(20.dp)
                    )
                }
            }

            // 2. Hero Header Plant Info
            Column(
                modifier = Modifier.fillMaxWidth(),
                verticalArrangement = Arrangement.spacedBy(AppSpacing.xs)
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
                                .size(56.dp)
                                .clip(CircleShape)
                                .background(AppColors.Primary.copy(alpha = 0.15f))
                                .border(1.dp, AppColors.Primary.copy(alpha = 0.4f), CircleShape),
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
                                    imageVector = Icons.Default.WaterDrop,
                                    contentDescription = null,
                                    tint = AppColors.Primary,
                                    modifier = Modifier.size(26.dp)
                                )
                            }
                        }

                        Column {
                            Text(
                                text = potName,
                                fontSize = 26.sp,
                                fontWeight = FontWeight.Bold,
                                color = AppColors.TextPrimary
                            )

                            val subtitleParts = listOfNotNull(
                                species?.takeIf { it.isNotBlank() },
                                location?.takeIf { it.isNotBlank() }
                            )

                            if (subtitleParts.isNotEmpty()) {
                                Text(
                                    text = subtitleParts.joinToString(" • "),
                                    fontSize = 13.sp,
                                    color = AppColors.TextSecondary,
                                    modifier = Modifier.padding(top = 2.dp)
                                )
                            }
                        }
                    }

                    Box(
                        modifier = Modifier
                            .size(36.dp)
                            .clip(CircleShape)
                            .background(AppColors.GlassBgStrong)
                            .border(1.dp, AppColors.GlassBorder, CircleShape)
                            .clickable { onRefreshClick() },
                        contentAlignment = Alignment.Center
                    ) {
                        Icon(
                            imageVector = Icons.Default.Refresh,
                            contentDescription = "Refresh",
                            tint = AppColors.TextSecondary,
                            modifier = Modifier.size(18.dp)
                        )
                    }
                }

                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(AppSpacing.xs),
                    modifier = Modifier.padding(top = AppSpacing.xs)
                ) {
                    Box(
                        modifier = Modifier
                            .size(8.dp)
                            .clip(CircleShape)
                            .background(globalHealth.color)
                    )
                    Text(
                        text = globalHealth.title,
                        fontSize = 13.sp,
                        fontWeight = FontWeight.Medium,
                        color = globalHealth.color
                    )
                }
            }

            // 3. Low Water Alert Banner
            if (isLowWater) {
                MinimalAlertBanner(message = "Water reservoir almost empty")
            }

            // 4. LIVE WEBSOCKET VITALITY RINGS (3 Anneaux pour les métriques)
            Text(
                text = "Live Plant Metrics",
                fontSize = 18.sp,
                fontWeight = FontWeight.Bold,
                color = AppColors.TextPrimary
            )

            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .clip(RoundedCornerShape(AppRadius.xl))
                    .background(AppColors.GlassBg)
                    .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.xl))
                    .padding(AppSpacing.lg)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceEvenly,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    LiveVitalityRing(
                        title = "Soil Moisture",
                        progress = ((soilMoisture ?: 0f) / 100f).coerceIn(0f, 1f),
                        valueText = soilMoisture?.let { "${it.toInt()}%" } ?: "--",
                        icon = Icons.Default.WaterDrop,
                        ringColor = AppColors.MoistureColor
                    )

                    LiveVitalityRing(
                        title = "Light Level",
                        progress = ((lightLevel ?: 0f) / 100f).coerceIn(0f, 1f),
                        valueText = lightLevel?.let { "${it.toInt()}%" } ?: "--",
                        icon = Icons.Default.WbSunny,
                        ringColor = AppColors.LightColor
                    )

                    LiveVitalityRing(
                        title = "Reservoir",
                        progress = ((waterLevel ?: 0f) / 100f).coerceIn(0f, 1f),
                        valueText = waterLevel?.let { "${it.toInt()}%" } ?: "--",
                        icon = Icons.Default.LocalDrink,
                        ringColor = AppColors.WaterColor
                    )
                }
            }

            // 5. CARTE DE CONTRÔLE DE LA POMPE & BOUTON D'ARROSAGE MANUEL
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .clip(RoundedCornerShape(AppRadius.xl))
                    .background(if (pumpState) AppColors.Success.copy(alpha = 0.15f) else AppColors.GlassBg)
                    .border(
                        1.dp,
                        if (pumpState) AppColors.Success.copy(alpha = 0.5f) else AppColors.GlassBorder,
                        RoundedCornerShape(AppRadius.xl)
                    )
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
                                .size(44.dp)
                                .clip(RoundedCornerShape(AppRadius.lg))
                                .background(
                                    if (pumpState) AppColors.Success.copy(alpha = 0.2f) else AppColors.PumpColor.copy(alpha = 0.15f)
                                ),
                            contentAlignment = Alignment.Center
                        ) {
                            Icon(
                                imageVector = Icons.Default.ElectricBolt,
                                contentDescription = null,
                                tint = if (pumpState) AppColors.Success else AppColors.PumpColor,
                                modifier = Modifier.size(22.dp)
                            )
                        }

                        Column {
                            Text(
                                text = "Watering Pump",
                                fontSize = 16.sp,
                                fontWeight = FontWeight.Bold,
                                color = AppColors.TextPrimary
                            )
                            Text(
                                text = if (pumpState) "Watering in progress..." else "Pump Standby (Auto)",
                                fontSize = 12.sp,
                                color = AppColors.TextSecondary
                            )
                        }
                    }

                    Box(
                        modifier = Modifier
                            .height(42.dp)
                            .clip(RoundedCornerShape(AppRadius.lg))
                            .background(if (isActuallyConnected) AppColors.Primary else AppColors.GlassBgStrong)
                            .border(
                                1.dp,
                                if (isActuallyConnected) AppColors.Primary else AppColors.GlassBorder,
                                RoundedCornerShape(AppRadius.lg)
                            )
                            .clickable(enabled = !isWateringLoading && isActuallyConnected) {
                                if (!isWateringLoading && isActuallyConnected) {
                                    isWateringLoading = true
                                    onWateringClick()
                                    coroutineScope.launch {
                                        delay(3000)
                                        isWateringLoading = false
                                    }
                                }
                            }
                            .padding(horizontal = AppSpacing.lg),
                        contentAlignment = Alignment.Center
                    ) {
                        if (isWateringLoading) {
                            CircularProgressIndicator(
                                color = AppColors.TextPrimary,
                                modifier = Modifier.size(18.dp),
                                strokeWidth = 2.dp
                            )
                        } else {
                            Row(
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.spacedBy(6.dp)
                            ) {
                                Icon(
                                    imageVector = Icons.Default.WaterDrop,
                                    contentDescription = null,
                                    tint = if (isActuallyConnected) AppColors.TextPrimary else AppColors.TextMuted,
                                    modifier = Modifier.size(16.dp)
                                )
                                Text(
                                    text = "Water",
                                    fontSize = 14.sp,
                                    fontWeight = FontWeight.Bold,
                                    color = if (isActuallyConnected) AppColors.TextPrimary else AppColors.TextMuted
                                )
                            }
                        }
                    }
                }
            }

            // 6. REAL BBD HISTOGRAM & ACTIVITY LOGS (Dynamique selon BBD réelle)
            RealBbdAnalyticsSection(
                history = history,
                currentMetric = metric
            )
        }
    }

    if (showSettingsDialog) {
        GlassEditPotDialog(
            currentName = potName,
            currentSpecies = species ?: "",
            currentLocation = location ?: "",
            onDismiss = { showSettingsDialog = false },
            onSave = { newName, newSpecies, newLoc ->
                showSettingsDialog = false
                onUpdatePot(newName, newSpecies, newLoc)
            },
            onDeleteClick = {
                showSettingsDialog = false
                showDeleteConfirmDialog = true
            }
        )
    }

    if (showDeleteConfirmDialog) {
        GlassConfirmDeleteDialog(
            potName = potName,
            onDismiss = { showDeleteConfirmDialog = false },
            onConfirmDelete = {
                showDeleteConfirmDialog = false
                onDeletePot()
            }
        )
    }
}

@Composable
private fun LiveVitalityRing(
    title: String,
    progress: Float,
    valueText: String,
    icon: ImageVector,
    ringColor: Color
) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.spacedBy(AppSpacing.xs)
    ) {
        Box(
            modifier = Modifier.size(72.dp),
            contentAlignment = Alignment.Center
        ) {
            CircularProgressIndicator(
                progress = { 1f },
                modifier = Modifier.fillMaxSize(),
                color = AppColors.GlassBorder,
                strokeWidth = 6.dp
            )

            CircularProgressIndicator(
                progress = { progress },
                modifier = Modifier.fillMaxSize(),
                color = ringColor,
                strokeWidth = 6.dp
            )

            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Icon(
                    imageVector = icon,
                    contentDescription = null,
                    tint = ringColor,
                    modifier = Modifier.size(18.dp)
                )
                Text(
                    text = valueText,
                    fontSize = 13.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.TextPrimary
                )
            }
        }

        Text(
            text = title,
            fontSize = 11.sp,
            fontWeight = FontWeight.Medium,
            color = AppColors.TextSecondary
        )
    }
}

@Composable
private fun RealBbdAnalyticsSection(
    history: List<MetricDto>,
    currentMetric: MetricDto?
) {
    val validHistory = history.ifEmpty {
        listOfNotNull(currentMetric)
    }

    val moistureMetrics = validHistory.filter { it.soilMoisture != null }

    Column(
        verticalArrangement = Arrangement.spacedBy(AppSpacing.md),
        modifier = Modifier.padding(top = AppSpacing.sm)
    ) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(AppSpacing.xs)
            ) {
                Icon(
                    imageVector = Icons.Default.Analytics,
                    contentDescription = null,
                    tint = AppColors.Primary,
                    modifier = Modifier.size(20.dp)
                )
                Text(
                    text = "Recorded Moisture Entries",
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.TextPrimary
                )
            }

            Text(
                text = "${moistureMetrics.size} record(s)",
                fontSize = 12.sp,
                color = AppColors.TextSecondary
            )
        }

        // HISTOGRAMME RÉEL BBD
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(AppRadius.xl))
                .background(AppColors.GlassBg)
                .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.xl))
                .padding(AppSpacing.lg)
        ) {
            Column(verticalArrangement = Arrangement.spacedBy(AppSpacing.md)) {
                if (moistureMetrics.isEmpty()) {
                    Text(
                        text = "No BBD moisture entries recorded yet.",
                        fontSize = 13.sp,
                        color = AppColors.TextSecondary,
                        modifier = Modifier.padding(vertical = AppSpacing.sm)
                    )
                } else {
                    val displayItems = moistureMetrics.takeLast(10)
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(85.dp),
                        horizontalArrangement = Arrangement.spacedBy(AppSpacing.sm),
                        verticalAlignment = Alignment.Bottom
                    ) {
                        displayItems.forEachIndexed { idx, metricItem ->
                            val value = metricItem.soilMoisture ?: 0f
                            val fillRatio = (value / 100f).coerceIn(0.08f, 1f)
                            val label = formatMetricDateLabel(metricItem, idx, displayItems.size)

                            Column(
                                horizontalAlignment = Alignment.CenterHorizontally,
                                verticalArrangement = Arrangement.spacedBy(4.dp),
                                modifier = Modifier.weight(1f)
                            ) {
                                Text(
                                    text = "${value.toInt()}%",
                                    fontSize = 10.sp,
                                    fontWeight = FontWeight.Bold,
                                    color = AppColors.TextPrimary
                                )
                                Box(
                                    modifier = Modifier
                                        .fillMaxWidth()
                                        .height((50 * fillRatio).dp)
                                        .clip(RoundedCornerShape(4.dp))
                                        .background(
                                            if (value < 20f) AppColors.Warning else AppColors.MoistureColor
                                        )
                                )
                                Text(
                                    text = label,
                                    fontSize = 9.sp,
                                    color = AppColors.TextSecondary,
                                    fontWeight = FontWeight.Medium
                                )
                            }
                        }
                    }
                }
            }
        }

        // REAL LOGS JOURNAL
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(AppRadius.xl))
                .background(AppColors.GlassBg)
                .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.xl))
                .padding(AppSpacing.lg)
        ) {
            Column(verticalArrangement = Arrangement.spacedBy(AppSpacing.sm)) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(AppSpacing.xs)
                ) {
                    Icon(
                        imageVector = Icons.Default.History,
                        contentDescription = null,
                        tint = AppColors.TextSecondary,
                        modifier = Modifier.size(16.dp)
                    )
                    Text(
                        text = "Activity Log Journal",
                        fontSize = 14.sp,
                        fontWeight = FontWeight.SemiBold,
                        color = AppColors.TextPrimary
                    )
                }

                val realLogs = computeRealLogs(validHistory, currentMetric)

                realLogs.forEach { logItem ->
                    ActivityLogRow(
                        icon = logItem.icon,
                        iconTint = logItem.color,
                        title = logItem.title,
                        timeText = logItem.timeText
                    )
                }
            }
        }
    }
}

private fun formatMetricDateLabel(metric: MetricDto, index: Int, total: Int): String {
    val dateStr = metric.createdAt ?: metric.timestamp ?: metric.time
    if (!dateStr.isNullOrBlank()) {
        try {
            if (dateStr.contains("T")) {
                val parts = dateStr.split("T")
                val datePart = parts[0]
                val monthDay = datePart.split("-").takeLast(2).joinToString("/")
                if (monthDay.isNotEmpty()) return monthDay
            } else if (dateStr.length >= 10) {
                val monthDay = dateStr.take(10).split("-").takeLast(2).joinToString("/")
                if (monthDay.isNotEmpty()) return monthDay
            }
        } catch (ignored: Exception) {}
    }
    return if (index == total - 1) "Now" else "#${index + 1}"
}

data class LogItem(
    val title: String,
    val timeText: String,
    val icon: ImageVector,
    val color: Color
)

private fun computeRealLogs(history: List<MetricDto>, currentMetric: MetricDto?): List<LogItem> {
    val list = mutableListOf<LogItem>()

    // Extract real logs from history records
    history.takeLast(5).reversed().forEach { record ->
        val timeLabel = formatLogTimeLabel(record)
        when {
            record.pumpState == true -> {
                list.add(
                    LogItem(
                        title = "Auto-watering triggered (5s)",
                        timeText = timeLabel,
                        icon = Icons.Default.WaterDrop,
                        color = AppColors.MoistureColor
                    )
                )
            }
            record.waterLevel != null && record.waterLevel <= 20f -> {
                list.add(
                    LogItem(
                        title = "Low water reservoir warning (${record.waterLevel.toInt()}%)",
                        timeText = timeLabel,
                        icon = Icons.Default.Warning,
                        color = AppColors.Warning
                    )
                )
            }
            record.soilMoisture != null -> {
                list.add(
                    LogItem(
                        title = "Soil moisture recorded (${record.soilMoisture.toInt()}%)",
                        timeText = timeLabel,
                        icon = Icons.Default.Spa,
                        color = AppColors.Success
                    )
                )
            }
        }
    }

    if (list.isEmpty()) {
        if (currentMetric?.pumpState == true) {
            list.add(
                LogItem(
                    title = "Auto-watering triggered (5s)",
                    timeText = "Today • Just now",
                    icon = Icons.Default.WaterDrop,
                    color = AppColors.MoistureColor
                )
            )
        }
        currentMetric?.soilMoisture?.let { sm ->
            list.add(
                LogItem(
                    title = "Live soil moisture update (${sm.toInt()}%)",
                    timeText = "Today • Live",
                    icon = Icons.Default.Spa,
                    color = AppColors.Success
                )
            )
        }
    }

    if (list.isEmpty()) {
        list.add(
            LogItem(
                title = "Pot monitoring active",
                timeText = "Today • Just now",
                icon = Icons.Default.Spa,
                color = AppColors.Primary
            )
        )
    }

    return list.distinctBy { it.title + it.timeText }
}

private fun formatLogTimeLabel(metric: MetricDto): String {
    val dateStr = metric.createdAt ?: metric.timestamp ?: metric.time
    if (!dateStr.isNullOrBlank()) {
        try {
            if (dateStr.contains("T")) {
                val parts = dateStr.split("T")
                val datePart = parts[0]
                val timePart = parts.getOrNull(1)?.take(5) ?: ""
                val monthDay = datePart.split("-").takeLast(2).joinToString("/")
                return "$monthDay • $timePart"
            }
        } catch (ignored: Exception) {}
    }
    return "Recent • BBD"
}

@Composable
private fun ActivityLogRow(
    icon: ImageVector,
    iconTint: Color,
    title: String,
    timeText: String
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 4.dp),
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.spacedBy(AppSpacing.md)
    ) {
        Box(
            modifier = Modifier
                .size(32.dp)
                .clip(CircleShape)
                .background(iconTint.copy(alpha = 0.15f)),
            contentAlignment = Alignment.Center
        ) {
            Icon(
                imageVector = icon,
                contentDescription = null,
                tint = iconTint,
                modifier = Modifier.size(16.dp)
            )
        }

        Column(modifier = Modifier.weight(1f)) {
            Text(
                text = title,
                fontSize = 13.sp,
                fontWeight = FontWeight.Medium,
                color = AppColors.TextPrimary
            )
            Text(
                text = timeText,
                fontSize = 10.sp,
                color = AppColors.TextTertiary
            )
        }
    }
}

@Composable
private fun MinimalAlertBanner(message: String) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .clip(RoundedCornerShape(AppRadius.lg))
            .background(AppColors.Warning.copy(alpha = 0.15f))
            .border(1.dp, AppColors.Warning.copy(alpha = 0.4f), RoundedCornerShape(AppRadius.lg))
            .padding(horizontal = AppSpacing.md, vertical = AppSpacing.sm),
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.spacedBy(AppSpacing.sm)
    ) {
        Icon(
            imageVector = Icons.Default.Warning,
            contentDescription = null,
            tint = AppColors.Warning,
            modifier = Modifier.size(18.dp)
        )
        Text(
            text = message,
            fontSize = 13.sp,
            fontWeight = FontWeight.Medium,
            color = AppColors.TextPrimary
        )
    }
}

@Composable
private fun GlassEditPotDialog(
    currentName: String,
    currentSpecies: String,
    currentLocation: String,
    onDismiss: () -> Unit,
    onSave: (name: String, species: String, location: String) -> Unit,
    onDeleteClick: () -> Unit
) {
    var name by remember { mutableStateOf(currentName) }
    var species by remember { mutableStateOf(currentSpecies) }
    var location by remember { mutableStateOf(currentLocation) }

    Dialog(onDismissRequest = onDismiss) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(AppRadius.xl))
                .background(Color(0xFF0F172A))
                .border(1.dp, AppColors.GlassBorderFocus, RoundedCornerShape(AppRadius.xl))
                .padding(AppSpacing.xl)
        ) {
            Column(verticalArrangement = Arrangement.spacedBy(AppSpacing.md)) {
                Text(
                    text = "Manage Pot",
                    fontSize = 20.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.TextPrimary
                )

                DialogInputField(
                    label = "Pot Name",
                    value = name,
                    onValueChange = { name = it }
                )
                Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                    Text(
                        text = "Plant Species",
                        fontSize = 12.sp,
                        color = AppColors.TextSecondary,
                        fontWeight = FontWeight.Medium
                    )
                    com.aurele.syncflorapotapp.ui.components.SearchablePlantDropdown(
                        value = species,
                        onValueChange = { species = it },
                        placeholder = "Search plant species"
                    )
                }
                DialogInputField(
                    label = "Location",
                    value = location,
                    onValueChange = { location = it }
                )

                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clip(RoundedCornerShape(AppRadius.md))
                        .background(AppColors.Error.copy(alpha = 0.15f))
                        .border(1.dp, AppColors.Error.copy(alpha = 0.3f), RoundedCornerShape(AppRadius.md))
                        .clickable { onDeleteClick() }
                        .padding(AppSpacing.md),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(AppSpacing.sm)
                ) {
                    Icon(
                        imageVector = Icons.Default.Delete,
                        contentDescription = "Delete",
                        tint = AppColors.Error,
                        modifier = Modifier.size(18.dp)
                    )
                    Text(
                        text = "Delete Pot",
                        color = AppColors.Error,
                        fontSize = 14.sp,
                        fontWeight = FontWeight.SemiBold
                    )
                }

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.End,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    TextButton(onClick = onDismiss) {
                        Text(text = "Cancel", color = AppColors.TextSecondary)
                    }
                    TextButton(
                        onClick = { onSave(name, species, location) },
                        enabled = name.trim().length > 1
                    ) {
                        Text(
                            text = "Save",
                            fontWeight = FontWeight.Bold,
                            color = AppColors.Primary
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun GlassConfirmDeleteDialog(
    potName: String,
    onDismiss: () -> Unit,
    onConfirmDelete: () -> Unit
) {
    Dialog(onDismissRequest = onDismiss) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(AppRadius.xl))
                .background(Color(0xFF0F172A))
                .border(1.dp, AppColors.GlassBorderFocus, RoundedCornerShape(AppRadius.xl))
                .padding(AppSpacing.xl)
        ) {
            Column(verticalArrangement = Arrangement.spacedBy(AppSpacing.md)) {
                Text(
                    text = "Delete Pot?",
                    fontSize = 18.sp,
                    fontWeight = FontWeight.Bold,
                    color = AppColors.TextPrimary
                )

                Text(
                    text = "Do you want to remove \"$potName\" from your devices? This action is permanent.",
                    color = AppColors.TextSecondary,
                    fontSize = 14.sp
                )

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.End,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    TextButton(onClick = onDismiss) {
                        Text(text = "Cancel", color = AppColors.TextPrimary)
                    }
                    TextButton(onClick = onConfirmDelete) {
                        Text(
                            text = "Delete",
                            color = AppColors.Error,
                            fontWeight = FontWeight.Bold
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun DialogInputField(
    label: String,
    value: String,
    onValueChange: (String) -> Unit
) {
    Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
        Text(
            text = label,
            fontSize = 12.sp,
            color = AppColors.TextSecondary,
            fontWeight = FontWeight.Medium
        )
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(44.dp)
                .clip(RoundedCornerShape(AppRadius.md))
                .background(AppColors.GlassBg)
                .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.md))
                .padding(horizontal = AppSpacing.md),
            contentAlignment = Alignment.CenterStart
        ) {
            BasicTextField(
                value = value,
                onValueChange = onValueChange,
                textStyle = TextStyle(
                    color = AppColors.TextPrimary,
                    fontSize = 14.sp,
                    fontWeight = FontWeight.Medium
                ),
                cursorBrush = SolidColor(AppColors.Primary),
                singleLine = true,
                modifier = Modifier.fillMaxWidth()
            )
        }
    }
}

data class GlobalHealthState(
    val title: String,
    val color: Color
)

private fun computeGlobalHealth(metrics: List<MetricUiModel>, isConnected: Boolean): GlobalHealthState {
    if (!isConnected) {
        return GlobalHealthState(
            title = "Pot Offline",
            color = AppColors.TextMuted
        )
    }

    val healthStatuses = metrics.map { getHealthStatus(it.key, it.value, it.numericValue) }

    return when {
        healthStatuses.contains(HealthStatus.CRITICAL) -> GlobalHealthState(
            title = "Action Required (Critical Level)",
            color = AppColors.Error
        )
        healthStatuses.contains(HealthStatus.WARNING) -> GlobalHealthState(
            title = "Warning (Attention Needed)",
            color = AppColors.Warning
        )
        else -> GlobalHealthState(
            title = "Optimal Health",
            color = AppColors.Success
        )
    }
}

private fun getHealthStatus(sensorKey: String, value: String, numericValue: Float?): HealthStatus {
    if (value == "--") return HealthStatus.CRITICAL

    val num = numericValue ?: value.replace("[^0-9.]".toRegex(), "").toFloatOrNull()
    val thresholds = SENSOR_THRESHOLDS[sensorKey] ?: return HealthStatus.HEALTHY

    if (num == null) return HealthStatus.HEALTHY

    return when {
        (num < thresholds.warningMin || num > thresholds.warningMax) -> HealthStatus.CRITICAL
        (num < thresholds.min || num > thresholds.max) -> HealthStatus.WARNING
        else -> HealthStatus.HEALTHY
    }
}

private fun getSensorStatusText(sensorKey: String, numericValue: Float?): String {
    if (numericValue == null) return "--"
    return when (sensorKey) {
        "soil_moisture" -> when {
            numericValue < 20f -> "Dry"
            numericValue > 80f -> "Very Wet"
            else -> "Optimal"
        }
        "light" -> when {
            numericValue < 10f -> "Dim"
            else -> "Adequate"
        }
        "water_level_state" -> when {
            numericValue <= 20f -> "Low"
            else -> "Sufficient"
        }
        else -> "Auto"
    }
}