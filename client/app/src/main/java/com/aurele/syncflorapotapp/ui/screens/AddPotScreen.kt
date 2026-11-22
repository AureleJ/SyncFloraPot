package com.aurele.syncflorapotapp.ui.screens

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothDevice
import android.bluetooth.le.ScanResult
import android.os.Build
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
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
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material.icons.filled.Bluetooth
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.Spa
import androidx.compose.material.icons.filled.Wifi
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
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
import com.espressif.provisioning.DeviceConnectionEvent
import com.espressif.provisioning.ESPConstants
import com.espressif.provisioning.ESPDevice
import com.espressif.provisioning.ESPProvisionManager
import com.espressif.provisioning.WiFiAccessPoint
import com.espressif.provisioning.listeners.BleScanListener
import com.espressif.provisioning.listeners.ProvisionListener
import com.espressif.provisioning.listeners.WiFiScanListener
import org.greenrobot.eventbus.EventBus
import org.greenrobot.eventbus.Subscribe
import org.greenrobot.eventbus.ThreadMode

enum class AddPotStep {
    SCAN_BLE,
    ENTER_PIN,
    SELECT_WIFI,
    DETAILS
}

data class DiscoveredEspDevice(
    val name: String,
    val device: BluetoothDevice,
    val serviceUuid: String?
)

@SuppressLint("MissingPermission")
@Composable
fun AddPotScreen(
    onBackClick: () -> Unit = {},
    onSavePot: (name: String, species: String, location: String, deviceId: String) -> Unit = { _, _, _, _ -> }
) {
    val context = LocalContext.current
    val provisionManager = remember { ESPProvisionManager.getInstance(context) }

    var step by remember { mutableStateOf(AddPotStep.SCAN_BLE) }
    var espDevice by remember { mutableStateOf<ESPDevice?>(null) }
    var selectedTarget by remember { mutableStateOf<DiscoveredEspDevice?>(null) }

    val discoveredDevices = remember { mutableStateListOf<DiscoveredEspDevice>() }
    val wifiNetworks = remember { mutableStateListOf<WiFiAccessPoint>() }

    var pinCode by remember { mutableStateOf("") }
    var selectedSsid by remember { mutableStateOf("") }
    var wifiPassword by remember { mutableStateOf("") }

    var potName by remember { mutableStateOf("") }
    var species by remember { mutableStateOf("") }
    var location by remember { mutableStateOf("") }

    var isLoading by remember { mutableStateOf(false) }
    var errorMessage by remember { mutableStateOf("") }

    DisposableEffect(Unit) {
        val eventSubscriber = object {
            @Subscribe(threadMode = ThreadMode.MAIN)
            fun onDeviceConnectionEvent(event: DeviceConnectionEvent) {
                when (event.eventType) {
                    ESPConstants.EVENT_DEVICE_CONNECTED -> {
                        espDevice?.setProofOfPossession(pinCode)
                        espDevice?.scanNetworks(object : WiFiScanListener {
                            override fun onWifiListReceived(wifiList: ArrayList<WiFiAccessPoint>?) {
                                isLoading = false
                                wifiNetworks.clear()
                                wifiList?.let { wifiNetworks.addAll(it) }
                                step = AddPotStep.SELECT_WIFI
                            }

                            override fun onWiFiScanFailed(e: Exception?) {
                                isLoading = false
                                errorMessage = "Failed to scan pot Wi-Fi."
                            }
                        })
                    }
                    ESPConstants.EVENT_DEVICE_CONNECTION_FAILED -> {
                        isLoading = false
                        errorMessage = "Bluetooth connection to pot failed."
                    }
                    ESPConstants.EVENT_DEVICE_DISCONNECTED -> {}
                }
            }
        }

        EventBus.getDefault().register(eventSubscriber)

        onDispose {
            EventBus.getDefault().unregister(eventSubscriber)
            try {
                provisionManager.stopBleScan()
                espDevice?.disconnectDevice()
            } catch (ignored: Exception) {}
        }
    }

    val permissionsToRequest = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
        arrayOf(
            Manifest.permission.BLUETOOTH_SCAN,
            Manifest.permission.BLUETOOTH_CONNECT,
            Manifest.permission.ACCESS_FINE_LOCATION
        )
    } else {
        arrayOf(Manifest.permission.ACCESS_FINE_LOCATION)
    }

    fun startBleScan() {
        errorMessage = ""
        isLoading = true
        discoveredDevices.clear()

        provisionManager.searchBleEspDevices("PROV_", object : BleScanListener {
            override fun scanStartFailed() {
                isLoading = false
                errorMessage = "Unable to start Bluetooth scan."
            }

            override fun onPeripheralFound(device: BluetoothDevice?, scanResult: ScanResult?) {
                val devName = device?.name ?: scanResult?.scanRecord?.deviceName
                if (!devName.isNullOrBlank() && device != null) {
                    if (discoveredDevices.none { it.device.address == device.address }) {
                        val uuid = scanResult?.scanRecord?.serviceUuids?.firstOrNull()?.uuid?.toString()
                        discoveredDevices.add(DiscoveredEspDevice(devName, device, uuid))
                    }
                }
            }

            override fun scanCompleted() {
                isLoading = false
            }

            override fun onFailure(e: Exception?) {
                isLoading = false
                errorMessage = e?.localizedMessage ?: "BLE scan error."
            }
        })
    }

    val permissionLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestMultiplePermissions()
    ) { perms ->
        if (perms.values.all { it }) {
            startBleScan()
        } else {
            errorMessage = "Bluetooth permissions required to detect pot."
        }
    }

    val canSubmitDetails = potName.trim().length > 1 &&
            species.trim().length > 1 &&
            location.trim().length > 1

    AppBackground {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(horizontal = AppSpacing.lg)
                .padding(top = AppSpacing.huge, bottom = AppSpacing.xl),
            verticalArrangement = Arrangement.spacedBy(AppSpacing.lg)
        ) {
            Box(
                modifier = Modifier
                    .size(40.dp)
                    .clip(CircleShape)
                    .background(AppColors.GlassBgStrong)
                    .border(1.dp, AppColors.GlassBorder, CircleShape)
                    .clickable {
                        if (step == AddPotStep.SCAN_BLE) {
                            onBackClick()
                        } else {
                            step = when (step) {
                                AddPotStep.ENTER_PIN -> AddPotStep.SCAN_BLE
                                AddPotStep.SELECT_WIFI -> AddPotStep.ENTER_PIN
                                AddPotStep.DETAILS -> AddPotStep.SELECT_WIFI
                                else -> AddPotStep.SCAN_BLE
                            }
                        }
                    },
                contentAlignment = Alignment.Center
            ) {
                Icon(
                    imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                    contentDescription = "Back",
                    tint = AppColors.TextPrimary,
                    modifier = Modifier.size(20.dp)
                )
            }

            when (step) {
                AddPotStep.SCAN_BLE -> {
                    StepTitle(
                        icon = Icons.Default.Bluetooth,
                        title = "Find Your Pot",
                        subtitle = "Turn on the pot and ensure Bluetooth is active (PROV_...)."
                    )

                    PrimaryButton(
                        label = "Scan for Pot",
                        onClick = { permissionLauncher.launch(permissionsToRequest) },
                        loading = isLoading
                    )

                    if (discoveredDevices.isEmpty()) {
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .clip(RoundedCornerShape(AppRadius.xl))
                                .background(AppColors.GlassBg)
                                .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.xl))
                                .padding(AppSpacing.xl),
                            contentAlignment = Alignment.Center
                        ) {
                            Text(
                                text = "No pot found at the moment.",
                                color = AppColors.TextSecondary,
                                textAlign = TextAlign.Center,
                                fontSize = 14.sp
                            )
                        }
                    } else {
                        LazyColumn(
                            modifier = Modifier.fillMaxWidth(),
                            verticalArrangement = Arrangement.spacedBy(AppSpacing.sm)
                        ) {
                            items(discoveredDevices) { item ->
                                Box(
                                    modifier = Modifier
                                        .fillMaxWidth()
                                        .clip(RoundedCornerShape(AppRadius.lg))
                                        .background(AppColors.GlassBg)
                                        .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.lg))
                                        .clickable {
                                            selectedTarget = item
                                            step = AddPotStep.ENTER_PIN
                                            errorMessage = ""
                                        }
                                        .padding(AppSpacing.md)
                                ) {
                                    Row(
                                        modifier = Modifier.fillMaxWidth(),
                                        verticalAlignment = Alignment.CenterVertically,
                                        horizontalArrangement = Arrangement.spacedBy(AppSpacing.md)
                                    ) {
                                        Box(
                                            modifier = Modifier
                                                .size(38.dp)
                                                .clip(RoundedCornerShape(AppRadius.md))
                                                .background(AppColors.Primary.copy(alpha = 0.15f)),
                                            contentAlignment = Alignment.Center
                                        ) {
                                            Icon(
                                                imageVector = Icons.Default.Spa,
                                                contentDescription = null,
                                                tint = AppColors.Primary,
                                                modifier = Modifier.size(20.dp)
                                            )
                                        }
                                        Text(
                                            text = item.name,
                                            color = AppColors.TextPrimary,
                                            fontSize = 15.sp,
                                            fontWeight = FontWeight.SemiBold,
                                            modifier = Modifier.weight(1f)
                                        )
                                        Icon(
                                            imageVector = Icons.AutoMirrored.Filled.KeyboardArrowRight,
                                            contentDescription = null,
                                            tint = AppColors.TextSecondary,
                                            modifier = Modifier.size(20.dp)
                                        )
                                    }
                                }
                            }
                        }
                    }
                }

                AddPotStep.ENTER_PIN -> {
                    StepTitle(
                        icon = Icons.Default.Lock,
                        title = "Validate Your Pot",
                        subtitle = "Enter the 4-digit PIN (Proof of Possession) configured on the ESP32."
                    )

                    GlassSimpleInput(
                        value = pinCode,
                        onValueChange = { if (it.length <= 4) pinCode = it },
                        placeholder = "4-digit PIN",
                        keyboardType = KeyboardType.Number
                    )

                    PrimaryButton(
                        label = "Validate & Connect",
                        disabled = pinCode.length != 4 || isLoading,
                        loading = isLoading,
                        onClick = {
                            val target = selectedTarget ?: return@PrimaryButton
                            isLoading = true
                            errorMessage = ""

                            val device = provisionManager.createESPDevice(
                                ESPConstants.TransportType.TRANSPORT_BLE,
                                ESPConstants.SecurityType.SECURITY_1
                            )
                            espDevice = device

                            device.connectBLEDevice(target.device, target.serviceUuid ?: "")
                        }
                    )
                }

                AddPotStep.SELECT_WIFI -> {
                    StepTitle(
                        icon = Icons.Default.Wifi,
                        title = "Connect Pot to Wi-Fi",
                        subtitle = "Select the Wi-Fi network for the pot to connect to."
                    )

                    LazyColumn(
                        modifier = Modifier
                            .fillMaxWidth()
                            .heightIn(max = 200.dp),
                        verticalArrangement = Arrangement.spacedBy(AppSpacing.xs)
                    ) {
                        items(wifiNetworks) { network ->
                            val isSelected = network.wifiName == selectedSsid
                            Box(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .clip(RoundedCornerShape(AppRadius.lg))
                                    .background(if (isSelected) AppColors.GlassBgStrong else AppColors.GlassBg)
                                    .border(
                                        1.dp,
                                        if (isSelected) AppColors.Primary else AppColors.GlassBorder,
                                        RoundedCornerShape(AppRadius.lg)
                                    )
                                    .clickable { selectedSsid = network.wifiName }
                                    .padding(AppSpacing.md)
                            ) {
                                Row(
                                    modifier = Modifier.fillMaxWidth(),
                                    horizontalArrangement = Arrangement.SpaceBetween,
                                    verticalAlignment = Alignment.CenterVertically
                                ) {
                                    Column {
                                        Text(
                                            text = network.wifiName,
                                            color = AppColors.TextPrimary,
                                            fontSize = 15.sp,
                                            fontWeight = FontWeight.Medium
                                        )
                                        Text(
                                            text = "${network.rssi} dBm",
                                            color = AppColors.TextTertiary,
                                            fontSize = 12.sp,
                                            modifier = Modifier.padding(top = 2.dp)
                                        )
                                    }
                                    if (isSelected) {
                                        Icon(
                                            imageVector = Icons.Default.Check,
                                            contentDescription = null,
                                            tint = AppColors.Success,
                                            modifier = Modifier.size(20.dp)
                                        )
                                    }
                                }
                            }
                        }
                    }

                    GlassSimpleInput(
                        value = wifiPassword,
                        onValueChange = { wifiPassword = it },
                        placeholder = "Wi-Fi Password",
                        visualTransformation = PasswordVisualTransformation(),
                        keyboardType = KeyboardType.Password
                    )

                    PrimaryButton(
                        label = "Send Credentials",
                        disabled = selectedSsid.isEmpty() || wifiPassword.isEmpty() || isLoading,
                        loading = isLoading,
                        onClick = {
                            val device = espDevice ?: return@PrimaryButton
                            isLoading = true
                            errorMessage = ""

                            device.provision(selectedSsid, wifiPassword, object : ProvisionListener {
                                override fun createSessionFailed(e: Exception?) {
                                    isLoading = false
                                    errorMessage = "Secure session creation failed."
                                }

                                override fun wifiConfigSent() {}

                                override fun wifiConfigFailed(e: Exception?) {
                                    isLoading = false
                                    errorMessage = "Failed to send Wi-Fi configuration."
                                }

                                override fun wifiConfigApplied() {}

                                override fun wifiConfigApplyFailed(e: Exception?) {
                                    isLoading = false
                                    errorMessage = "Pot failed to apply Wi-Fi configuration."
                                }

                                override fun provisioningFailedFromDevice(reason: ESPConstants.ProvisionFailureReason?) {
                                    isLoading = false
                                    errorMessage = "Wi-Fi error from pot: ${reason?.name}"
                                }

                                override fun deviceProvisioningSuccess() {
                                    isLoading = false
                                    step = AddPotStep.DETAILS
                                }

                                override fun onProvisioningFailed(e: Exception?) {
                                    isLoading = false
                                    errorMessage = e?.localizedMessage ?: "Provisioning failed."
                                }
                            })
                        }
                    )
                }

                AddPotStep.DETAILS -> {
                    Column(
                        modifier = Modifier
                            .weight(1f)
                            .verticalScroll(rememberScrollState()),
                        verticalArrangement = Arrangement.spacedBy(AppSpacing.md)
                    ) {
                        StepTitle(
                            icon = Icons.Default.Spa,
                            title = "Name Your Pot",
                            subtitle = "The pot is connected to Wi-Fi. Save it to your account."
                        )

                        FieldGroup(
                            label = "Pot Name",
                            value = potName,
                            onValueChange = { potName = it },
                            placeholder = "e.g. Monstera Living Room"
                        )

                        Column(verticalArrangement = Arrangement.spacedBy(AppSpacing.xs)) {
                            Text(
                                text = "Plant Species",
                                color = AppColors.TextSecondary,
                                fontSize = 12.sp,
                                fontWeight = FontWeight.Medium
                            )
                            com.aurele.syncflorapotapp.ui.components.SearchablePlantDropdown(
                                value = species,
                                onValueChange = { species = it },
                                placeholder = "Search plant (e.g. Monstera Deliciosa)"
                            )
                        }

                        FieldGroup(
                            label = "Location",
                            value = location,
                            onValueChange = { location = it },
                            placeholder = "e.g. Living Room"
                        )

                        Spacer(modifier = Modifier.height(AppSpacing.sm))

                        PrimaryButton(
                            label = "Add to My Account",
                            disabled = !canSubmitDetails || isLoading,
                            loading = isLoading,
                            onClick = {
                                val targetName = selectedTarget?.name ?: "PROV_UNKNOWN"
                                onSavePot(potName, species, location, targetName)
                            }
                        )
                    }
                }
            }

            if (errorMessage.isNotEmpty()) {
                Text(
                    text = errorMessage,
                    color = AppColors.Error,
                    fontSize = 12.sp,
                    textAlign = TextAlign.Center,
                    modifier = Modifier.fillMaxWidth()
                )
            }
        }
    }
}

@Composable
private fun StepTitle(icon: ImageVector, title: String, subtitle: String) {
    Column(
        verticalArrangement = Arrangement.spacedBy(AppSpacing.xs),
        modifier = Modifier.padding(bottom = AppSpacing.sm)
    ) {
        Box(
            modifier = Modifier
                .size(48.dp)
                .clip(RoundedCornerShape(AppRadius.lg))
                .background(AppColors.Primary.copy(alpha = 0.2f))
                .border(1.dp, AppColors.Primary.copy(alpha = 0.4f), RoundedCornerShape(AppRadius.lg)),
            contentAlignment = Alignment.Center
        ) {
            Icon(
                imageVector = icon,
                contentDescription = null,
                tint = AppColors.Primary,
                modifier = Modifier.size(24.dp)
            )
        }
        Text(
            text = title,
            fontSize = 24.sp,
            fontWeight = FontWeight.Bold,
            color = AppColors.TextPrimary
        )
        Text(
            text = subtitle,
            fontSize = 14.sp,
            color = AppColors.TextSecondary
        )
    }
}

@Composable
private fun FieldGroup(
    label: String,
    value: String,
    onValueChange: (String) -> Unit,
    placeholder: String
) {
    Column(verticalArrangement = Arrangement.spacedBy(AppSpacing.xs)) {
        Text(
            text = label,
            color = AppColors.TextSecondary,
            fontSize = 12.sp,
            fontWeight = FontWeight.Medium
        )
        GlassSimpleInput(
            value = value,
            onValueChange = onValueChange,
            placeholder = placeholder
        )
    }
}

@Composable
private fun GlassSimpleInput(
    value: String,
    onValueChange: (String) -> Unit,
    placeholder: String,
    keyboardType: KeyboardType = KeyboardType.Text,
    visualTransformation: VisualTransformation = VisualTransformation.None
) {
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
            onValueChange = onValueChange,
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
}

@Composable
private fun PrimaryButton(
    label: String,
    onClick: () -> Unit,
    disabled: Boolean = false,
    loading: Boolean = false
) {
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(50.dp)
            .clip(RoundedCornerShape(AppRadius.lg))
            .background(
                if (disabled) AppColors.Primary.copy(alpha = 0.45f) else AppColors.Primary
            )
            .border(1.dp, AppColors.GlassBorder, RoundedCornerShape(AppRadius.lg))
            .clickable(enabled = !disabled && !loading) { onClick() },
        contentAlignment = Alignment.Center
    ) {
        if (loading) {
            CircularProgressIndicator(
                color = AppColors.TextPrimary,
                modifier = Modifier.size(20.dp),
                strokeWidth = 2.dp
            )
        } else {
            Text(
                text = label,
                color = AppColors.TextPrimary,
                fontSize = 14.sp,
                fontWeight = FontWeight.SemiBold
            )
        }
    }
}