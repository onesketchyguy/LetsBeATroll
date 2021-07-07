#include "olcNoiseMaker.h"
struct EnvelopeADSR {
	double attackTime;
	double decayTime;
	double releaseTime;

	double sustainAmplitude;
	double startAmplitude;

	double triggerOnTime; // Press
	double triggerOffTime; // Release

	bool active;

	EnvelopeADSR() {
		attackTime = 0.01;
		decayTime = 0.01;
		releaseTime = 0.02;

		sustainAmplitude = 0.8;
		startAmplitude = 1.0;

		triggerOnTime = 0.0;
		triggerOffTime = 0.0;
		active = false;
	}

	double GetAmplitude(double time) {
		double amplitude = 0.0;
		double lifeTime = time - triggerOnTime;

		if (active) {
			// Attack
			if (lifeTime < attackTime)
				amplitude = (lifeTime / attackTime) * startAmplitude;

			// Decay
			if (lifeTime > attackTime && lifeTime < (attackTime + decayTime))
				amplitude = ((lifeTime - attackTime) / decayTime) *
				(sustainAmplitude - startAmplitude) + startAmplitude;

			// Sustain
			if (lifeTime > (attackTime + decayTime)) {
				amplitude = sustainAmplitude;
			}
		}
		else {
			// Release
			amplitude = ((time - triggerOffTime) / releaseTime) * (0.0 - sustainAmplitude) + sustainAmplitude;
		}

		if (amplitude <= 0.001) {
			amplitude = 0.0;
		}

		return amplitude;
	}

	void NoteOn(double time) {
		triggerOnTime = time;
		active = true;
	}

	void NoteOff(double time) {
		triggerOffTime = time;
		active = false;
	}
};

atomic<double> frequency = 0.0;
EnvelopeADSR envelope;

olcNoiseMaker<short>* voice;

enum class WaveType {
	SIN, // Sine wave
	SQR, // Square wave
	TRI, // Triangle wave
	SAW,
	OSAW, // Mathimatically optimised saw
	PNO // Pseudo random noise
};

/// <summary>
/// Converts frequency(Hz) to angular velocity
/// </summary>
/// <param name="hertz"></param>
/// <returns></returns>
double w(double hertz) {
	return hertz * 2 * PI;
}

/// <summary>
/// oscillator
/// </summary>
/// <returns></returns>
double osc(double hertz, double time, WaveType type) {
	switch (type) {
	case WaveType::SIN:
		return sin(w(hertz) * time);
	case WaveType::SQR:
		return sin(w(hertz) * time) > 0 ? 0.1 : -0.1;
	case WaveType::TRI:
		return asin(sin(w(hertz) * time)) * 2.0 / PI;
	case WaveType::SAW:
	{
		double output = 0.0;

		for (double i = 1.0; i < 20.0; i++)
			output += sin(i * w(hertz) * time) / i;

		return 0.5 * (output * (2.0 / PI));
	}
	case WaveType::OSAW:
		return 0.5 * ((2.0 / PI) * (hertz * PI * fmod(time, 1.0 / hertz) - (PI / 2.0)));
	case WaveType::PNO:
		return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;
	default:
		return 0.0;
	}
}

byte soundKey[16];
int currentKey = -1;

bool playMusic = true;
double lastNotePlayed;

const int songLength = 24;

byte* song;
double songNoteTime = 1;
int currentNote = -1;

double MakeNoise(double time) {
	double output = envelope.GetAmplitude(time) * (
		osc(frequency * .5, time, WaveType::SAW)
		+ osc(frequency, time, WaveType::SQR)
		);

	return output * 0.5;
}

void InitializeSounds() {
	vector<wstring> soundDevices = olcNoiseMaker<short>::Enumerate();
	for (auto d : soundDevices) wcout << "Found output device: " << d << endl;

	voice = new olcNoiseMaker<short>(soundDevices[0], 44100, 1, 8, 512);
	voice->SetUserFunction(MakeNoise);

	envelope.attackTime = .05;
	envelope.releaseTime = .3;
	envelope.decayTime = 0.1;

	song = new byte[songLength]{
		0, 2, 0, 3, 0, 2, 1, 3,
		0, 2, 0, 3, 0, 2, 0, 3,
		0, 4, 2, 3, 5, 2, 4, 6
	};
}

void PlaySFX(int hertz) {
	if (playMusic == false) {
		frequency = 0;

		return;
	}
}

void UpdateSounds() {
	double octaveBaseFrequency = 110;
	double rootOf2_12th = pow(2.0, 1.0 / 12);
	bool keyPressed = false;

	if (playMusic == false) {
		frequency = 0;

		return;
	}

	if (voice->GetTime() > lastNotePlayed && playMusic == 1) {
		if (currentNote >= 0)
			soundKey[song[currentNote]] = 0;

		currentNote++;
		if (currentNote >= songLength)currentNote = 0;

		soundKey[song[currentNote]] = 1;
		lastNotePlayed = voice->GetTime() + (songNoteTime);
	}
	else if (playMusic == 0) {
		soundKey[song[currentNote]] = 0;
	}

	for (int k = 0; k < 16; k++)
	{
		//if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
		if (soundKey[k] != 0)
		{
			if (currentKey != k)
			{
				frequency = octaveBaseFrequency * pow(rootOf2_12th, k);
				envelope.NoteOn(voice->GetTime());
				wcout << "\rNote On : " << voice->GetTime() << "s " << frequency << "Hz " << k;
				currentKey = k;
			}

			keyPressed = true;
		}
	}

	if (!keyPressed)
	{
		if (currentKey != -1)
		{
			wcout << "\rNote Off: " << voice->GetTime() << "s                        ";
			envelope.NoteOff(voice->GetTime());
			currentKey = -1;
		}
	}
}