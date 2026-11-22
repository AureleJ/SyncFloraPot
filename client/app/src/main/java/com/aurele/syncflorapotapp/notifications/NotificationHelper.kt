package com.aurele.syncflorapotapp.notifications

import android.Manifest
import android.annotation.SuppressLint
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat
import com.aurele.syncflorapotapp.MainActivity
import com.aurele.syncflorapotapp.R

class NotificationHelper(private val context: Context) {

    companion object {
        const val CHANNEL_ID = "syncflorapot_alerts_channel"
        const val CHANNEL_NAME = "Plant & Pot Alerts"
        const val NOTIFICATION_COOLDOWN_MS = 3600000L
    }

    private val lastNotifiedSoil = mutableMapOf<String, Long>()
    private val lastNotifiedWater = mutableMapOf<String, Long>()

    init {
        createNotificationChannel()
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                CHANNEL_ID,
                CHANNEL_NAME,
                NotificationManager.IMPORTANCE_HIGH
            ).apply {
                description = "Notifications when your plant needs water or the reservoir is empty"
                enableVibration(true)
            }

            val notificationManager =
                context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            notificationManager.createNotificationChannel(channel)
        }
    }

    fun checkAndNotifyMetrics(potId: String, potName: String, soilMoisture: Float?, waterLevel: Float?) {
        val now = System.currentTimeMillis()

        if (soilMoisture != null && soilMoisture < 20f) {
            val lastTime = lastNotifiedSoil[potId] ?: 0L
            if (now - lastTime > NOTIFICATION_COOLDOWN_MS) {
                lastNotifiedSoil[potId] = now
                showNotification(
                    id = potId.hashCode(),
                    title = "Water Needed 💧",
                    message = "The soil for \"$potName\" is dry (${soilMoisture.toInt()}%). Consider watering!"
                )
            }
        }

        if (waterLevel != null && waterLevel <= 20f) {
            val lastTime = lastNotifiedWater[potId] ?: 0L
            if (now - lastTime > NOTIFICATION_COOLDOWN_MS) {
                lastNotifiedWater[potId] = now
                showNotification(
                    id = potId.hashCode() + 1,
                    title = "Water Reservoir Low ⚠️",
                    message = "The reservoir for \"$potName\" is almost empty (${waterLevel.toInt()}%). Please refill."
                )
            }
        }
    }

    @SuppressLint("MissingPermission")
    private fun showNotification(id: Int, title: String, message: String) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            if (ActivityCompat.checkSelfPermission(
                    context,
                    Manifest.permission.POST_NOTIFICATIONS
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                return
            }
        }

        val intent = Intent(context, MainActivity::class.java).apply {
            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        }

        val pendingIntent = PendingIntent.getActivity(
            context,
            0,
            intent,
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )

        val builder = NotificationCompat.Builder(context, CHANNEL_ID)
            .setSmallIcon(R.mipmap.ic_launcher)
            .setContentTitle(title)
            .setContentText(message)
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true)
            .setContentIntent(pendingIntent)

        try {
            NotificationManagerCompat.from(context).notify(id, builder.build())
        } catch (ignored: Exception) {}
    }
}