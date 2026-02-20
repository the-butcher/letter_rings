package at.the_butchers.letter_rings

import android.Manifest
import android.content.Context
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import android.os.Handler
import android.os.Looper
import android.os.Process
import android.util.Log
import androidx.annotation.RequiresPermission
import androidx.work.Worker
import androidx.work.WorkerParameters
import com.shazam.shazamkit.AudioSampleRateInHz
import com.shazam.shazamkit.MatchResult
import com.shazam.shazamkit.ShazamCatalog
import com.shazam.shazamkit.ShazamKit
import com.shazam.shazamkit.ShazamKitResult
import kotlinx.coroutines.runBlocking
import java.nio.ByteBuffer

class WordPairWorker(appContext: Context, workerParams: WorkerParameters): Worker(appContext, workerParams) {

    @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
    override fun doWork(): Result {

        Log.d(WORK_LOG_TAG, "do word pair work")

        var wordL = "WEST"
        var wordR = "EAST"

        // TODO :: pick a new pair of words

        Log.d(WORK_LOG_TAG, "wordL ($wordL), wordR ($wordL)")

        // let the WorkHandler know that this instance is done working, must be async or there are issues cancelling instances
        Handler(Looper.getMainLooper()).postDelayed(  {
            WorkHandler.onDoWordPairWork(wordL, wordR) // let WorkHandler take further action
        }, 100)

        return Result.success()

    }


    private suspend fun recognizeFromBytes(catalog: ShazamCatalog, micRecording: ByteArray): MatchResult {

        val signatureGenerator = (ShazamKit.createSignatureGenerator(AudioSampleRateInHz.SAMPLE_RATE_48000) as ShazamKitResult.Success).data

        signatureGenerator.append(micRecording, micRecording.size, System.currentTimeMillis())
        val signature = signatureGenerator.generateSignature()
        Log.i(RGNZ_LOG_TAG, "got signature (${signature.durationInMs}ms)")

        val session = (ShazamKit.createSession(catalog) as ShazamKitResult.Success).data
        val matchResult = session.match(signature)


        Log.d("MIC", matchResult.toString())

        return matchResult

    }

    @RequiresPermission(allOf=[Manifest.permission.RECORD_AUDIO])
    private suspend fun simpleMicRecording(catalog: ShazamCatalog) : ByteArray{

        val audioSource = MediaRecorder.AudioSource.UNPROCESSED

        val audioFormat = AudioFormat.Builder()
            .setChannelMask(AudioFormat.CHANNEL_IN_MONO)
            .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
            .setSampleRate(48_000)
            .build()

        val audioRecord = AudioRecord.Builder()
            .setAudioSource(audioSource)
            .setAudioFormat(audioFormat)
            .build()

        val seconds = 10 // catalog.maximumQuerySignatureDurationInMs

        // Final desired buffer size to allocate 12 seconds of audio
        val size = audioFormat.sampleRate * audioFormat.encoding.toByteAllocation() * seconds
        val destination = ByteBuffer.allocate(size.toInt())

        // Small buffer to retrieve chunks of audio
        val bufferSize = AudioRecord.getMinBufferSize(
            48_000,
            AudioFormat.CHANNEL_IN_MONO,
            AudioFormat.ENCODING_PCM_16BIT
        )

        // Make sure you are on a dedicated thread or thread pool for mic recording only and
        // elevate the priority to THREAD_PRIORITY_URGENT_AUDIO
        Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO)

        audioRecord.startRecording()
        val readBuffer = ByteArray(bufferSize)
        while (destination.remaining()>0) {
            val actualRead = audioRecord.read(readBuffer, 0, bufferSize)
            val byteArray = readBuffer.sliceArray(0 until actualRead)
            destination.putTrimming(byteArray)
        }
        audioRecord.release()
        return destination.array()

    }

    private fun Int.toByteAllocation(): Int {
        return when (this) {
            AudioFormat.ENCODING_PCM_16BIT -> 2
            else -> throw IllegalArgumentException("Unsupported encoding")
        }
    }

    fun ByteBuffer.putTrimming(byteArray: ByteArray) {
        if (byteArray.size <= this.capacity() - this.position()) {
            this.put(byteArray)
        } else {
            this.put(byteArray, 0, this.capacity() - this.position())
        }
    }

}