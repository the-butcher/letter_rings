package at.the_butchers.letter_rings

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.work.Worker
import androidx.work.WorkerParameters

class WordPairWorker(appContext: Context, workerParams: WorkerParameters): Worker(appContext, workerParams) {

    val WORDS: List<Pair<String, String>> = listOf(
        Pair("LOVE", "HATE"),
        Pair("REAL", "FAKE"),
        Pair("WARM", "COLD"),
        Pair("MANY", "NONE"),
        Pair("FAST", "SLOW"),
        Pair("KIND", "GRIM"),
        Pair("GOOD", "EVIL"),
        Pair("KEEP", "GIVE"),
        Pair("WELL", "SICK"),
        Pair("HOPE", "FEAR"),
        Pair("FACT", "MYTH"),
        Pair("RISE", "FALL"),
        Pair("MORE", "LESS"),
        Pair("MOVE", "STOP"),
        Pair("REST", "WORK"),
        Pair("HEAL", "HURT"),
        Pair("LIFT", "DROP"),
        Pair("SOFT", "HARD"),
        Pair("CALM", "LOUD"),
        Pair("RICH", "POOR"),
        Pair("PURE", "FOUL"),
        Pair("NICE", "MEAN"),
        Pair("PEAK", "BASE"),
        Pair("PULL", "PUSH"),
        Pair("LAMB", "WOLF"),
        Pair("HUGE", "TINY"),
        Pair("FULL", "VOID"),
        Pair("STAY", "FLEE"),
        Pair("UNDO", "REDO"),
        Pair("LIVE", "PASS"),
        Pair("EARN", "LOSE"),
        Pair("GAIN", "LOSS")
    );

    @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
    override fun doWork(): Result {

        Log.d(LOG_TAG_WORK, "do word pair work")

        val randomWords = WORDS.random();

        Log.d(LOG_TAG_WORK, "wordL ($randomWords.first), wordR ($randomWords.second)")

        // let the WorkHandler know that this instance is done working, must be async or there are issues cancelling instances
        Handler(Looper.getMainLooper()).postDelayed(  {
            WorkHandler.onDoWordPairWork(randomWords.first, randomWords.second) // let WorkHandler take further action
        }, 100)

        return Result.success()

    }

}