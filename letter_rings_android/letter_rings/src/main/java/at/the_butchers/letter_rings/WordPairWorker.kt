package at.the_butchers.letter_rings

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.work.Worker
import androidx.work.WorkerParameters

class WordPairWorker(appContext: Context, workerParams: WorkerParameters): Worker(appContext, workerParams) {

    val wordPairs: List<Pair<String, String>> = listOf(
        Pair("STEW", "SOUP"),
        Pair("LOVE", "HATE"),
        Pair("REAL", "FAKE"),
        Pair("WARM", "COLD"),
        Pair("MANY", "NONE"),
        Pair("FAST", "SLOW"),
        Pair("KIND", "GRIM"),
        Pair("GOOD", "EVIL"),
        Pair("TAKE", "GIVE"),
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
        Pair("TALL", "TINY"),
        Pair("FULL", "HALF"),
        Pair("STAY", "FLEE"),
        Pair("UNDO", "REDO"),
        Pair("LIVE", "PASS"),
        Pair("FIND", "LOSE"),
        Pair("GAIN", "LOSS"),
        Pair("OPEN", "SHUT"),
        Pair("EAST", "WEST"),
        Pair("HIGH", "DEEP"),
        Pair("DAWN", "DUSK"),
        Pair("ROCK", "ROLL"),
        Pair("GOLD", "IRON"),
        Pair("SAFE", "RISK"),
        Pair("GROW", "FADE"),
        Pair("CODE", "DATA"),
        Pair("ROPE", "KNOT"),
        Pair("BEER", "WINE"),
        Pair("SKIN", "BONE"),
        Pair("FACE", "MASK"),
        Pair("MIND", "BODY"),
        Pair("HIDE", "SEEK"),
        Pair("LUCK", "FATE"),
        Pair("WEAK", "FIRM"),
    )

    @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
    override fun doWork(): Result {

        Log.d(LOG_TAG_WORK, "do word pair work")

        val randomWords = wordPairs.random()

        Log.d(LOG_TAG_WORK, "wordL ($randomWords.first), wordR ($randomWords.second)")

        // let the WorkHandler know that this instance is done working, must be async or there are issues cancelling instances
        Handler(Looper.getMainLooper()).postDelayed(  {
            WorkHandler.onDoWordPairWork(randomWords.first, randomWords.second) // let WorkHandler take further action
        }, 100)

        return Result.success()

    }

}