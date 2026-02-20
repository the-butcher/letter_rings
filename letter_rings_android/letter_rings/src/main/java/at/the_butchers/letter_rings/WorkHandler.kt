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

    fun scheduleWordPairWork() {

        cancelPreviousWork(WORK___WORDS)

        val constraints = Constraints.Builder()
            .setRequiredNetworkType(NetworkType.CONNECTED)
            .build()

        val workRequest = OneTimeWorkRequestBuilder<WordPairWorker>()
            .setInitialDelay(10, TimeUnit.SECONDS)
            .setConstraints(constraints)
            .addTag(WORK___WORDS)
            .build()

        val workManagerInstance = WorkManager.getInstance(MainActivity.instance.get() as Context)
        workManagerInstance.enqueue(workRequest)

        Log.i(WORK_LOG_TAG, "... word pair work scheduled")

    }

    fun scheduleShazamWork() {

        cancelPreviousWork(WORK__SHAZAM)

        val constraints = Constraints.Builder()
            .setRequiredNetworkType(NetworkType.CONNECTED)
            .build()

        val workRequest = OneTimeWorkRequestBuilder<ShazamWorker>()
            .setInitialDelay(60, TimeUnit.SECONDS)
            .setConstraints(constraints)
            .addTag(WORK__SHAZAM)
            .build()

        val workManagerInstance = WorkManager.getInstance(MainActivity.instance.get() as Context)
        workManagerInstance.enqueue(workRequest)

        Log.i(WORK_LOG_TAG, "... shazam work scheduled")

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun onDoWordPairWork(wordL: String, wordR: String) {
        try {

            val activityState = MainActivity.instance.get()?.lifecycle?.currentState

            MainActivity.instance.get()?.updateWords(wordL, wordR)
            Log.i(WORK_LOG_TAG, "scheduling word pair work (state: $activityState) ...")
            scheduleWordPairWork()

        } catch (ex: Exception) {
            Log.e(WORK_LOG_TAG, ex.toString())
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun onDoShazamWork(title: String, artist: String, valid: Boolean) {
        try {

            val activityState = MainActivity.instance.get()?.lifecycle?.currentState

            MainActivity.instance.get()?.updateLabels(title, artist, valid)
            Log.i(WORK_LOG_TAG, "scheduling shazam work (state: $activityState) ...")
            scheduleShazamWork()

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

    fun cancelPreviousWork(workTag: String) {

        val workManagerInstance = WorkManager.getInstance(MainActivity.instance.get() as Context)
        workManagerInstance.cancelAllWorkByTag(workTag)
        Log.i(WORK_LOG_TAG, "... work cancelled")

    }

}