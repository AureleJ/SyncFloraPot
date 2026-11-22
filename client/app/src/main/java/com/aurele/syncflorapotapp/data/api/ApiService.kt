package com.aurele.syncflorapotapp.data.api

import com.aurele.syncflorapotapp.data.local.SessionManager
import com.google.gson.annotations.SerializedName
import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.Body
import retrofit2.http.DELETE
import retrofit2.http.GET
import retrofit2.http.POST
import retrofit2.http.PUT
import retrofit2.http.Path

data class AuthRequest(
    val email: String,
    val password: String
)

data class UserDto(
    val id: String?,
    val email: String
)

data class AuthResponse(
    val token: String,
    val user: UserDto
)

data class PotDto(
    val id: String,
    val name: String,
    val location: String?,
    val connected: Boolean = false,
    val species: String? = null,
    @SerializedName("device_id") val deviceId: String? = null
)

data class CreatePotRequest(
    val name: String,
    val species: String,
    val location: String,
    val deviceId: String
)

data class UpdatePotRequest(
    val name: String? = null,
    val location: String? = null,
    val species: String? = null
)

data class DeletePotResponse(
    val message: String,
    val id: String
)

data class MetricDto(
    val time: String? = null,
    @SerializedName("created_at") val createdAt: String? = null,
    @SerializedName("timestamp") val timestamp: String? = null,
    @SerializedName("pot_id") val potId: String? = null,
    @SerializedName("soil_moisture") val soilMoisture: Float? = null,
    val light: Float? = null,
    @SerializedName("water_level") val waterLevel: Float? = null,
    @SerializedName("pump_state") val pumpState: Boolean? = null
)

interface ApiService {
    @POST("auth/login")
    suspend fun login(@Body request: AuthRequest): AuthResponse

    @POST("auth/register")
    suspend fun register(@Body request: AuthRequest): AuthResponse

    @GET("pots")
    suspend fun getPots(): List<PotDto>

    @POST("pots")
    suspend fun createPot(@Body request: CreatePotRequest): PotDto

    @PUT("pots/{id}")
    suspend fun updatePot(@Path("id") potId: String, @Body request: UpdatePotRequest): PotDto

    @DELETE("pots/{id}")
    suspend fun deletePot(@Path("id") potId: String): DeletePotResponse

    @GET("pots/{id}/latest")
    suspend fun getLatestMetric(@Path("id") potId: String): MetricDto?

    @GET("pots/{id}/metrics")
    suspend fun getPotMetricsHistory(@Path("id") potId: String): List<MetricDto>
}

object RetrofitClient {
    private const val BASE_URL = "https://syncflorapot-api.aurelejoblet.com"

    fun create(sessionManager: SessionManager): ApiService {
        val logging = HttpLoggingInterceptor().apply {
            level = HttpLoggingInterceptor.Level.BODY
        }

        val okHttpClient = OkHttpClient.Builder()
            .addInterceptor(logging)
            .addInterceptor { chain ->
                val requestBuilder = chain.request().newBuilder()
                sessionManager.getAuthToken()?.let { token ->
                    requestBuilder.addHeader("Authorization", "Bearer $token")
                }
                chain.proceed(requestBuilder.build())
            }
            .build()

        return Retrofit.Builder()
            .baseUrl(BASE_URL)
            .client(okHttpClient)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
            .create(ApiService::class.java)
    }
}