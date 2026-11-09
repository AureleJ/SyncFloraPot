package com.aurele.syncflorapotapp.data.api

import com.google.gson.annotations.SerializedName
import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.GET
import retrofit2.http.Query

data class PerenualImage(
    @SerializedName("original_url") val originalUrl: String? = null,
    @SerializedName("regular_url") val regularUrl: String? = null,
    @SerializedName("medium_url") val mediumUrl: String? = null,
    @SerializedName("thumbnail") val thumbnail: String? = null
)

data class PerenualPlantDto(
    val id: Int,
    @SerializedName("common_name") val commonName: String? = null,
    @SerializedName("scientific_name") val scientificName: List<String>? = null,
    val watering: String? = null,
    @SerializedName("default_image") val defaultImage: PerenualImage? = null
)

data class PerenualResponse(
    val data: List<PerenualPlantDto>? = null
)

interface PlantApiService {
    @GET("species-list")
    suspend fun searchPlants(
        @Query("key") apiKey: String = "sk-0b1a64f48394a2026",
        @Query("q") query: String
    ): PerenualResponse
}

object PlantRetrofitClient {
    private const val BASE_URL = "https://perenual.com/api/"

    val api: PlantApiService by lazy {
        val logging = HttpLoggingInterceptor().apply {
            level = HttpLoggingInterceptor.Level.BODY
        }

        val okHttpClient = OkHttpClient.Builder()
            .addInterceptor(logging)
            .build()

        Retrofit.Builder()
            .baseUrl(BASE_URL)
            .client(okHttpClient)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
            .create(PlantApiService::class.java)
    }
}