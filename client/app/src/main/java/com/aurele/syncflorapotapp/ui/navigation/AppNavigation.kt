package com.aurele.syncflorapotapp.ui.navigation

import android.net.Uri
import androidx.compose.animation.AnimatedContentTransitionScope
import androidx.compose.animation.core.tween
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.NavType
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import androidx.navigation.navArgument
import com.aurele.syncflorapotapp.ui.AppViewModel
import com.aurele.syncflorapotapp.ui.screens.AddPotScreen
import com.aurele.syncflorapotapp.ui.screens.DashboardScreen
import com.aurele.syncflorapotapp.ui.screens.LoginScreen
import com.aurele.syncflorapotapp.ui.screens.PotDetailsScreen
import com.aurele.syncflorapotapp.ui.theme.AppColors

sealed class Screen(val route: String) {
    data object Login : Screen("login")
    data object Dashboard : Screen("dashboard")
    data object AddPot : Screen("add_pot")
    data object PotDetails : Screen("pot_details/{potId}/{potName}") {
        fun createRoute(potId: String, potName: String): String {
            return "pot_details/$potId/${Uri.encode(potName)}"
        }
    }
}

@Composable
fun AppNavigation(viewModel: AppViewModel = viewModel()) {
    val navController = rememberNavController()

    val isLoggedIn by viewModel.isLoggedIn.collectAsState()
    val userEmail by viewModel.userEmail.collectAsState()
    val pots by viewModel.pots.collectAsState()
    val isLoading by viewModel.isLoading.collectAsState()
    val errorMessage by viewModel.errorMessage.collectAsState()

    val startDestination = if (isLoggedIn) Screen.Dashboard.route else Screen.Login.route

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(AppColors.Background)
    ) {
        NavHost(
            navController = navController,
            startDestination = startDestination,
            modifier = Modifier.background(AppColors.Background),
            enterTransition = {
                slideIntoContainer(
                    towards = AnimatedContentTransitionScope.SlideDirection.Left,
                    animationSpec = tween(300)
                ) + fadeIn(animationSpec = tween(300))
            },
            exitTransition = {
                slideOutOfContainer(
                    towards = AnimatedContentTransitionScope.SlideDirection.Left,
                    animationSpec = tween(300)
                ) + fadeOut(animationSpec = tween(300))
            },
            popEnterTransition = {
                slideIntoContainer(
                    towards = AnimatedContentTransitionScope.SlideDirection.Right,
                    animationSpec = tween(300)
                ) + fadeIn(animationSpec = tween(300))
            },
            popExitTransition = {
                slideOutOfContainer(
                    towards = AnimatedContentTransitionScope.SlideDirection.Right,
                    animationSpec = tween(300)
                ) + fadeOut(animationSpec = tween(300))
            }
        ) {
            composable(
                route = Screen.Login.route,
                enterTransition = { fadeIn(animationSpec = tween(300)) },
                exitTransition = { fadeOut(animationSpec = tween(300)) }
            ) {
                LoginScreen(
                    isLoading = isLoading,
                    errorMessage = errorMessage,
                    onLoginClick = { email, pass ->
                        viewModel.login(email, pass) {
                            navController.navigate(Screen.Dashboard.route) {
                                popUpTo(Screen.Login.route) { inclusive = true }
                            }
                        }
                    },
                    onRegisterClick = { email, pass ->
                        viewModel.register(email, pass) {
                            navController.navigate(Screen.Dashboard.route) {
                                popUpTo(Screen.Login.route) { inclusive = true }
                            }
                        }
                    },
                    onClearError = { viewModel.clearError() }
                )
            }

            composable(
                route = Screen.Dashboard.route,
                enterTransition = { fadeIn(animationSpec = tween(300)) },
                exitTransition = {
                    slideOutOfContainer(
                        towards = AnimatedContentTransitionScope.SlideDirection.Left,
                        animationSpec = tween(300)
                    ) + fadeOut(animationSpec = tween(300))
                },
                popEnterTransition = {
                    slideIntoContainer(
                        towards = AnimatedContentTransitionScope.SlideDirection.Right,
                        animationSpec = tween(300)
                    ) + fadeIn(animationSpec = tween(300))
                }
            ) {
                DashboardScreen(
                    pots = pots,
                    userEmail = userEmail,
                    isLoading = isLoading,
                    onLogoutClick = {
                        viewModel.logout {
                            navController.navigate(Screen.Login.route) {
                                popUpTo(Screen.Dashboard.route) { inclusive = true }
                            }
                        }
                    },
                    onPotClick = { potId, potName ->
                        navController.navigate(Screen.PotDetails.createRoute(potId, potName))
                    },
                    onAddPotClick = {
                        navController.navigate(Screen.AddPot.route)
                    }
                )
            }

            composable(Screen.AddPot.route) {
                AddPotScreen(
                    onBackClick = { navController.popBackStack() },
                    onSavePot = { name, species, location, deviceId ->
                        viewModel.createPot(name, species, location, deviceId) {
                            navController.popBackStack()
                        }
                    }
                )
            }

            composable(
                route = Screen.PotDetails.route,
                arguments = listOf(
                    navArgument("potId") { type = NavType.StringType },
                    navArgument("potName") { type = NavType.StringType }
                )
            ) { backStackEntry ->
                val potId = backStackEntry.arguments?.getString("potId") ?: ""
                val potName = Uri.decode(backStackEntry.arguments?.getString("potName") ?: "Pot")

                androidx.compose.runtime.LaunchedEffect(potId) {
                    viewModel.loadLatestMetricForPot(potId)
                }

                val latestMetrics by viewModel.latestMetrics.collectAsState()
                val metricsHistory by viewModel.metricsHistory.collectAsState()
                val potMetric = latestMetrics[potId]
                val potHistory = metricsHistory[potId] ?: emptyList()
                val pot = pots.find { it.id == potId }

                val isConnected = pot?.connected == true

                PotDetailsScreen(
                    potId = potId,
                    potName = pot?.name ?: potName,
                    species = pot?.species,
                    location = pot?.location,
                    isPotConnected = isConnected,
                    metric = potMetric,
                    history = potHistory,
                    onBackClick = { navController.popBackStack() },
                    onRefreshClick = { viewModel.loadLatestMetricForPot(potId) },
                    onWateringClick = {
                        viewModel.loadLatestMetricForPot(potId)
                    },
                    onUpdatePot = { newName, newSpecies, newLocation ->
                        viewModel.updatePot(potId, newName, newSpecies, newLocation) {
                            viewModel.fetchPots()
                        }
                    },
                    onDeletePot = {
                        viewModel.deletePot(potId) {
                            navController.popBackStack()
                        }
                    }
                )
            }
        }
    }
}