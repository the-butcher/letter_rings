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

class ShazamWorker(appContext: Context, workerParams: WorkerParameters): Worker(appContext, workerParams) {

    @RequiresPermission(allOf = [Manifest.permission.RECORD_AUDIO, Manifest.permission.BLUETOOTH_CONNECT])
    override fun doWork(): Result {

        Log.d(WORK_LOG_TAG, "do work")

        var artist = "NA"
        var title = "NA"
        var valid = false

        if (MainActivity.instance.get()?.isRecognitionChecked == true) {

            Log.i(RGNZ_LOG_TAG, "recognition is active")

            val catalog = MainActivity.instance.get()?.catalog
            if (catalog != null) {
                runBlocking  {

                    val micRecording = simpleMicRecording(catalog)
                    Log.i(RGNZ_LOG_TAG, "got micRecording (${micRecording.size}bytes)")

                    val matchResult = recognizeFromBytes(catalog, micRecording)
                    Log.i(RGNZ_LOG_TAG, "got matchResult (is match: ${matchResult is MatchResult.Match})")

                    if (matchResult is MatchResult.Match) {
                        for (matchedMediaItem in matchResult.matchedMediaItems) {
                            artist = matchedMediaItem.artist ?: "NA (no-artist-value)"
                            title = matchedMediaItem.title ?: "NA (no-title-value)"
                            valid = matchedMediaItem.title != null && matchedMediaItem.artist != null
                        }
                    } else {
                        artist = "NA (no-artist-match)"
                        title = "NA (no-title-match)"
                    }
                }

            } else {

                Log.i(RGNZ_LOG_TAG, "catalog is null")

                artist = "NA (no-artist-catalog)"
                title = "NA (no-title-catalog)"

            }

        } else {

            Log.i(RGNZ_LOG_TAG, "recognition is inactive")

            artist = "NA (no-artist-recognition)"
            title = "NA (no-title-recognition)"

        }

        // let the WorkHandler know that this instance is done working, must be async or there are issues cancelling instances
        Handler(Looper.getMainLooper()).postDelayed( {
            WorkHandler.onDoWork(title, artist, valid) // let WorkHandler take further action
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
        android.os.Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO)

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