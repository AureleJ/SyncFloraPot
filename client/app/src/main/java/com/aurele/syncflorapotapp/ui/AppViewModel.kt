package com.aurele.syncflorapotapp.ui

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aurele.syncflorapotapp.data.api.ApiService
import com.aurele.syncflorapotapp.data.api.AuthRequest
import com.aurele.syncflorapotapp.data.api.CreatePotRequest
import com.aurele.syncflorapotapp.data.api.MetricDto
import com.aurele.syncflorapotapp.data.api.PotDto
import com.aurele.syncflorapotapp.data.api.RetrofitClient
import com.aurele.syncflorapotapp.data.local.SessionManager
import com.aurele.syncflorapotapp.notifications.NotificationHelper
import io.socket.client.IO
import io.socket.client.Socket
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.json.JSONObject

class AppViewModel(application: Application) : AndroidViewModel(application) {
    private val sessionManager = SessionManager(application)
    private val notificationHelper = NotificationHelper(application)
    private val api: ApiService = RetrofitClient.create(sessionManager)
    private var socket: Socket? = null

    private val _isLoggedIn = MutableStateFlow(sessionManager.isLoggedIn())
    val isLoggedIn: StateFlow<Boolean> = _isLoggedIn.asStateFlow()

    private val _userEmail = MutableStateFlow(sessionManager.getUserEmail() ?: "")
    val userEmail: StateFlow<String> = _userEmail.asStateFlow()

    private val _pots = MutableStateFlow<List<PotDto>>(emptyList())
    val pots: StateFlow<List<PotDto>> = _pots.asStateFlow()

    private val _latestMetrics = MutableStateFlow<Map<String, MetricDto>>(emptyMap())
    val latestMetrics: StateFlow<Map<String, MetricDto>> = _latestMetrics.asStateFlow()

    private val _metricsHistory = MutableStateFlow<Map<String, List<MetricDto>>>(emptyMap())
    val metricsHistory: StateFlow<Map<String, List<MetricDto>>> = _metricsHistory.asStateFlow()

    private val _isLoading = MutableStateFlow(false)
    val isLoading: StateFlow<Boolean> = _isLoading.asStateFlow()

    private val _errorMessage = MutableStateFlow<String?>(null)
    val errorMessage: StateFlow<String?> = _errorMessage.asStateFlow()

    init {
        if (sessionManager.isLoggedIn()) {
            fetchPots()
            connectSocket()
        }
    }

    private fun connectSocket() {
        val token = sessionManager.getAuthToken() ?: return
        try {
            socket?.disconnect()
            val options = IO.Options().apply {
                query = "token=$token"
            }
            socket = IO.socket("https://syncflorapot-api.aurelejoblet.com", options)

            socket?.on("syncFloraPotData") { args ->
                if (args.isNotEmpty()) {
                    val data = args[0] as? JSONObject ?: return@on
                    val potId = data.optString("potId")
                    if (potId.isNotEmpty()) {
                        val metric = MetricDto(
                            potId = potId,
                            soilMoisture = data.optDouble("soil_moisture", Double.NaN).toFloat().takeIf { !it.isNaN() },
                            light = data.optDouble("light", Double.NaN).toFloat().takeIf { !it.isNaN() },
                            waterLevel = data.optDouble("water_level", Double.NaN).toFloat().takeIf { !it.isNaN() },
                            pumpState = if (data.has("pump_state")) data.optBoolean("pump_state") else null
                        )
                        _latestMetrics.value = _latestMetrics.value + (potId to metric)
                        _pots.value = _pots.value.map { p ->
                            if (p.id == potId) p.copy(connected = true) else p
                        }
                        val existingHistory = _metricsHistory.value[potId] ?: emptyList()
                        _metricsHistory.value = _metricsHistory.value + (potId to (existingHistory + metric))
                        val pName = _pots.value.find { it.id == potId }?.name ?: "Votre pot"
                        notificationHelper.checkAndNotifyMetrics(
                            potId = potId,
                            potName = pName,
                            soilMoisture = metric.soilMoisture,
                            waterLevel = metric.waterLevel
                        )
                    }
                }
            }

            socket?.connect()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun login(email: String, pass: String, onSuccess: () -> Unit) {
        viewModelScope.launch {
            _isLoading.value = true
            _errorMessage.value = null
            try {
                val res = api.login(AuthRequest(email.trim(), pass.trim()))
                sessionManager.saveAuthToken(res.token)
                sessionManager.saveUserEmail(res.user.email)
                _userEmail.value = res.user.email
                _isLoggedIn.value = true
                fetchPots()
                connectSocket()
                onSuccess()
            } catch (e: Exception) {
                _errorMessage.value = e.localizedMessage ?: "Échec de connexion"
            } finally {
                _isLoading.value = false
            }
        }
    }

    fun register(email: String, pass: String, onSuccess: () -> Unit) {
        viewModelScope.launch {
            _isLoading.value = true
            _errorMessage.value = null
            try {
                api.register(AuthRequest(email.trim(), pass.trim()))
                login(email, pass, onSuccess)
            } catch (e: Exception) {
                _errorMessage.value = e.localizedMessage ?: "Échec de l'inscription"
            } finally {
                _isLoading.value = false
            }
        }
    }

    fun fetchPots() {
        viewModelScope.launch {
            _isLoading.value = true
            try {
                val list = api.getPots()
                _pots.value = list
            } catch (e: Exception) {
                _errorMessage.value = "Impossible de récupérer les pots"
            } finally {
                _isLoading.value = false
            }
        }
    }

    fun loadLatestMetricForPot(potId: String) {
        viewModelScope.launch {
            try {
                val metric = api.getLatestMetric(potId)
                if (metric != null) {
                    _latestMetrics.value = _latestMetrics.value + (potId to metric)
                    val pName = _pots.value.find { it.id == potId }?.name ?: "Votre pot"
                    notificationHelper.checkAndNotifyMetrics(
                        potId = potId,
                        potName = pName,
                        soilMoisture = metric.soilMoisture,
                        waterLevel = metric.waterLevel
                    )
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }
            try {
                val history = api.getPotMetricsHistory(potId)
                _metricsHistory.value = _metricsHistory.value + (potId to history)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    fun createPot(
        name: String,
        species: String,
        location: String,
        deviceId: String,
        onSuccess: () -> Unit
    ) {
        viewModelScope.launch {
            _isLoading.value = true
            _errorMessage.value = null
            try {
                api.createPot(
                    CreatePotRequest(
                        name = name.trim(),
                        species = species.trim(),
                        location = location.trim(),
                        deviceId = deviceId.trim()
                    )
                )
                fetchPots()
                onSuccess()
            } catch (e: Exception) {
                _errorMessage.value = e.localizedMessage ?: "Erreur d'ajout du pot"
            } finally {
                _isLoading.value = false
            }
        }
    }

    fun updatePot(
        potId: String,
        name: String,
        species: String,
        location: String,
        onSuccess: () -> Unit
    ) {
        viewModelScope.launch {
            _isLoading.value = true
            _errorMessage.value = null
            try {
                api.updatePot(
                    potId = potId,
                    request = com.aurele.syncflorapotapp.data.api.UpdatePotRequest(
                        name = name.trim().takeIf { it.isNotEmpty() },
                        species = species.trim().takeIf { it.isNotEmpty() },
                        location = location.trim().takeIf { it.isNotEmpty() }
                    )
                )
                fetchPots()
                onSuccess()
            } catch (e: Exception) {
                _errorMessage.value = e.localizedMessage ?: "Failed to update pot"
            } finally {
                _isLoading.value = false
            }
        }
    }

    fun deletePot(
        potId: String,
        onSuccess: () -> Unit
    ) {
        viewModelScope.launch {
            _isLoading.value = true
            _errorMessage.value = null
            try {
                api.deletePot(potId)
                _pots.value = _pots.value.filterNot { it.id == potId }
                _latestMetrics.value = _latestMetrics.value - potId
                fetchPots()
                onSuccess()
            } catch (e: Exception) {
                _errorMessage.value = e.localizedMessage ?: "Failed to delete pot"
            } finally {
                _isLoading.value = false
            }
        }
    }

    fun logout(onLoggedOut: () -> Unit) {
        socket?.disconnect()
        sessionManager.clearSession()
        _isLoggedIn.value = false
        _userEmail.value = ""
        _pots.value = emptyList()
        _latestMetrics.value = emptyMap()
        onLoggedOut()
    }

    fun clearError() {
        _errorMessage.value = null
    }

    override fun onCleared() {
        super.onCleared()
        socket?.disconnect()
    }
}