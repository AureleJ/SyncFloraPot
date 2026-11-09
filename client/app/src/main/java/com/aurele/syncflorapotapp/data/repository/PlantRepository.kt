package com.aurele.syncflorapotapp.data.repository

import com.aurele.syncflorapotapp.data.api.PlantRetrofitClient
import com.aurele.syncflorapotapp.data.local.PlantCatalog
import com.aurele.syncflorapotapp.data.local.PlantSpeciesInfo
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

object PlantRepository {

    private val cachedOnlinePlants = mutableMapOf<String, PlantSpeciesInfo>()

    suspend fun searchPlants(query: String): List<PlantSpeciesInfo> = withContext(Dispatchers.IO) {
        val localMatches = PlantCatalog.searchPlants(query)

        try {
            if (query.length >= 2) {
                val response = PlantRetrofitClient.api.searchPlants(query = query.trim())
                val onlineResults = response.data?.mapNotNull { item ->
                    val sciName = item.scientificName?.firstOrNull() ?: item.commonName ?: return@mapNotNull null
                    val imageUrl = item.defaultImage?.regularUrl
                        ?: item.defaultImage?.mediumUrl
                        ?: item.defaultImage?.originalUrl
                        ?: return@mapNotNull null

                    val wateringTip = when (item.watering?.lowercase()) {
                        "frequent" -> "Frequent watering needed"
                        "average" -> "Average watering (every 5-7 days)"
                        "minimum" -> "Low watering required"
                        else -> "Moderate watering"
                    }

                    PlantSpeciesInfo(
                        name = sciName,
                        commonName = item.commonName ?: sciName,
                        imageUrl = imageUrl,
                        wateringTip = wateringTip
                    ).also {
                        cachedOnlinePlants[sciName.lowercase()] = it
                    }
                } ?: emptyList()

                if (onlineResults.isNotEmpty()) {
                    return@withContext (localMatches + onlineResults).distinctBy { it.name.lowercase() }
                }
            }
        } catch (ignored: Exception) {}

        return@withContext localMatches
    }

    fun findPlant(query: String?): PlantSpeciesInfo? {
        if (query.isNullOrBlank()) return null
        val lower = query.lowercase().trim()
        return cachedOnlinePlants[lower] ?: PlantCatalog.findPlant(query)
    }
}