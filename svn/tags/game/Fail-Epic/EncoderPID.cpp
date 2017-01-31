/*#include "EncoderPID.h"

EncoderPIDSource::EncoderPIDSource(Encoder* encoder) {
	this->encoder = encoder;
}

EncoderPIDSource::~EncoderPIDSource() {
	delete encoder;
}

float EncoderPIDSource::PIDGet() {
	return encoder->Get(); // get returns 4 X position
}

EncoderPIDOutput::EncoderPIDOutput(SpeedController* motor) {
	this->motor = motor;
}

EncoderPIDOutput::~EncoderPIDOutput() {
	delete motor;
}

void EncoderPIDOutput::PIDWrite(float output) {
	motor->Set(output); // not GetDistance() ?
}
*/

