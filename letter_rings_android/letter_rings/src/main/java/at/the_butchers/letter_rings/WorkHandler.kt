package at.the_butchers.letter_rings

import android.Manifest
import android.content.Context
import android.util.Log
import androidx.annotation.RequiresPermission
import androidx.work.Constraints
import androidx.work.NetworkType
import androidx.work.OneTimeWorkRequestBuilder
import androidx.work.WorkManager
import java.util.concurrent.TimeUnit


object WorkHandler {

    // https://www.tothenew.com/blog/using-work-managers-periodic-work-for-less-than-15-minutes/

    fun scheduleWork() {

        cancelPreviousWork()

        val constraints = Constraints.Builder()
//            .setRequiresBatteryNotLow(true)
            .setRequiredNetworkType(NetworkType.CONNECTED)
            .build()

        val workRequest = OneTimeWorkRequestBuilder<ShazamWorker>()
            .setInitialDelay(60, TimeUnit.SECONDS)
            .setConstraints(constraints)
            .addTag(WORK__SHAZAM)
            .build()

        val workManagerInstance = WorkManager.getInstance(MainActivity.instance.get() as Context)
        workManagerInstance.enqueue(workRequest)

        Log.i(WORK_LOG_TAG, "... work scheduled")

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun onDoWork(title: String, artist: String, valid: Boolean) {
        try {

            val activityState = MainActivity.instance.get()?.lifecycle?.currentState

            MainActivity.instance.get()?.updateTxRecognition(title, artist, valid)
            Log.i(WORK_LOG_TAG, "scheduling work (state: $activityState) ...")
            scheduleWork()

//            if (activityState?.isAtLeast(Lifecycle.State.RESUMED) == true) {
//
//                MainActivity.instance.get()?.updateTxRecognition(title, artist)
//
//                Log.i(WORK_LOG_TAG, "scheduling work (UI active) ...")
//                scheduleWork()
//
//            } else if (activityState?.isAtLeast(Lifecycle.State.DESTROYED) == true) {
//
//                MainActivity.instance.get()?.updateTxRecognition(title, artist)
//
//                Log.i(WORK_LOG_TAG, "scheduling work (UI inactive) ...")
//                scheduleWork()
//
//            } else {
//
//                MainActivity.instance.get()?.updateTxRecognition(title, artist)
//
//                Log.i(WORK_LOG_TAG, "scheduling work (UI destroyed) ...")
//                scheduleWork()
//
////                Log.i(WORK_LOG_TAG, "cancelling work ...")
////                cancelPreviousWork()
//
//            }

        } catch (ex: Exception) {
            Log.e(WORK_LOG_TAG, ex.toString())
        }

    }

    fun cancelPreviousWork() {

        val workManagerInstance = WorkManager.getInstance(MainActivity.instance.get() as Context)
        workManagerInstance.cancelAllWorkByTag(WORK__SHAZAM)
        Log.i(WORK_LOG_TAG, "... work cancelled")

    }

}