package com.aurele.syncflorapotapp.data.local

data class PlantSpeciesInfo(
    val name: String,
    val commonName: String,
    val imageUrl: String,
    val minMoisture: Float = 20f,
    val maxMoisture: Float = 70f,
    val wateringTip: String = "Arrosage modéré"
)

object PlantCatalog {
    val plants = listOf(
        PlantSpeciesInfo(
            name = "Monstera Deliciosa",
            commonName = "Faux philodendron",
            imageUrl = "https://images.unsplash.com/photo-1614594975525-e45190c55d0b?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Ficus Lyrata",
            commonName = "Figuier lyre",
            imageUrl = "https://images.unsplash.com/photo-1597055181300-e3633a207518?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Ficus Elastica",
            commonName = "Caoutchouc",
            imageUrl = "https://images.unsplash.com/photo-1600411833196-7c1f6b1a8b90?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Epipremnum Aureum",
            commonName = "Pothos",
            imageUrl = "https://images.unsplash.com/photo-1617173944883-6ffbd35d584d?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Sansevieria Trifasciata",
            commonName = "Langue de belle-mère / Snake Plant",
            imageUrl = "https://images.unsplash.com/photo-1593482892290-f54927ae1bf6?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Calathea Orbifolia",
            commonName = "Calathea",
            imageUrl = "https://images.unsplash.com/photo-1599598425947-320f8c857ecb?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Calathea Zebrina",
            commonName = "Calathea zébrée",
            imageUrl = "https://images.unsplash.com/photo-1599598425947-320f8c857ecb?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Pilea Peperomioides",
            commonName = "Monnaie chinoise",
            imageUrl = "https://images.unsplash.com/photo-1592150621744-aca64f48394a?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Aloe Vera",
            commonName = "Aloe",
            imageUrl = "https://images.unsplash.com/photo-1596547609652-9cf5d8d76921?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Spathiphyllum",
            commonName = "Fleur de lune / Peace Lily",
            imageUrl = "https://images.unsplash.com/photo-1593691509543-c55fb32e7355?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Zamioculcas Zamiifolia",
            commonName = "Plante ZZ",
            imageUrl = "https://images.unsplash.com/photo-1637967886160-fd78dc3eb3f5?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Phalaenopsis",
            commonName = "Orchidée",
            imageUrl = "https://images.unsplash.com/photo-1525310072745-f49212b5ac6d?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Crassula Ovata",
            commonName = "Arbre de jade",
            imageUrl = "https://images.unsplash.com/photo-1509423350716-97f9360b4e09?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Chlorophytum Comosum",
            commonName = "Plante araignée / Spider Plant",
            imageUrl = "https://images.unsplash.com/photo-1572688484438-313a6e50c333?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Syngonium Podophyllum",
            commonName = "Syngonium",
            imageUrl = "https://images.unsplash.com/photo-1614594975525-e45190c55d0b?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Philodendron Hederaceum",
            commonName = "Philodendron scandens",
            imageUrl = "https://images.unsplash.com/photo-1617173944883-6ffbd35d584d?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Philodendron Birkin",
            commonName = "Philodendron Birkin",
            imageUrl = "https://images.unsplash.com/photo-1614594975525-e45190c55d0b?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Tradescantia Zebrina",
            commonName = "Misère violette",
            imageUrl = "https://images.unsplash.com/photo-1592150621744-aca64f48394a?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Euphorbia Trigona",
            commonName = "Cactus cathédrale",
            imageUrl = "https://images.unsplash.com/photo-1509423350716-97f9360b4e09?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Begonia Maculata",
            commonName = "Bégonia tamaya / à pois",
            imageUrl = "https://images.unsplash.com/photo-1599598425947-320f8c857ecb?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Alocasia Polly",
            commonName = "Alocasia Oreille d'éléphant",
            imageUrl = "https://images.unsplash.com/photo-1614594975525-e45190c55d0b?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Anthurium Andreanum",
            commonName = "Anthurium",
            imageUrl = "https://images.unsplash.com/photo-1525310072745-f49212b5ac6d?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Pachira Aquatica",
            commonName = "Arbre à argent / Money Tree",
            imageUrl = "https://images.unsplash.com/photo-1600411833196-7c1f6b1a8b90?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Strelitzia Nicolai",
            commonName = "Oiseau du paradis géant",
            imageUrl = "https://images.unsplash.com/photo-1614594975525-e45190c55d0b?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Ceropegia Woodii",
            commonName = "Chaîne de cœurs",
            imageUrl = "https://images.unsplash.com/photo-1592150621744-aca64f48394a?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Senecio Rowleyanus",
            commonName = "Collier de perles",
            imageUrl = "https://images.unsplash.com/photo-1509423350716-97f9360b4e09?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Dracaena Marginata",
            commonName = "Dragonnier",
            imageUrl = "https://images.unsplash.com/photo-1600411833196-7c1f6b1a8b90?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Aglaonema Commutatum",
            commonName = "Aglaonème",
            imageUrl = "https://images.unsplash.com/photo-1599598425947-320f8c857ecb?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Maranta Leuconeura",
            commonName = "Plante prière",
            imageUrl = "https://images.unsplash.com/photo-1592150621744-aca64f48394a?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Schefflera Arboricola",
            commonName = "Arbre ombrelle",
            imageUrl = "https://images.unsplash.com/photo-1597055181300-e3633a207518?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Asplenium Nidus",
            commonName = "Fougère nid d'oiseau",
            imageUrl = "https://images.unsplash.com/photo-1572688484438-313a6e50c333?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Nephrolepis Exaltata",
            commonName = "Fougère de Boston",
            imageUrl = "https://images.unsplash.com/photo-1572688484438-313a6e50c333?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Hoya Carnosa",
            commonName = "Fleur de porcelaine",
            imageUrl = "https://images.unsplash.com/photo-1525310072745-f49212b5ac6d?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Haworthia Fasciata",
            commonName = "Plante zèbre",
            imageUrl = "https://images.unsplash.com/photo-1509423350716-97f9360b4e09?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Chamaedorea Elegans",
            commonName = "Palmier d'intérieur",
            imageUrl = "https://images.unsplash.com/photo-1597055181300-e3633a207518?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Peperomia Caperata",
            commonName = "Pépéromia plissé",
            imageUrl = "https://images.unsplash.com/photo-1592150621744-aca64f48394a?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Oxalis Triangularis",
            commonName = "Trèfle pourpre",
            imageUrl = "https://images.unsplash.com/photo-1592150621744-aca64f48394a?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Hedera Helix",
            commonName = "Lierre d'intérieur",
            imageUrl = "https://images.unsplash.com/photo-1617173944883-6ffbd35d584d?w=400&auto=format&fit=crop"
        ),
        PlantSpeciesInfo(
            name = "Bonsai Ficus",
            commonName = "Bonsaï Ficus Microcarpa",
            imageUrl = "https://images.unsplash.com/photo-1512428813834-c702c7702b78?w=400&auto=format&fit=crop"
        )
    )

    fun findPlant(query: String?): PlantSpeciesInfo? {
        if (query.isNullOrBlank()) return null
        return plants.find {
            it.name.contains(query, ignoreCase = true) ||
            it.commonName.contains(query, ignoreCase = true)
        }
    }

    fun searchPlants(query: String): List<PlantSpeciesInfo> {
        if (query.isBlank()) return plants
        return plants.filter {
            it.name.contains(query, ignoreCase = true) ||
            it.commonName.contains(query, ignoreCase = true)
        }
    }
}