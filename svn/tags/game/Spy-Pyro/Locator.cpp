#include "Locator.h"
#include <math.h>

#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078640628620899

Locator* const locator = new Locator();
#define PI 3.1415926

void Locator::update()
{
	// DOES NOT TAKE STRAFING INTO ACCOUNT
	float FL = hal->getFLEncoder();
	float FR = hal->getFREncoder();
	float BL = hal->getBLEncoder();
	float BR = hal->getBREncoder();
	
	float dFL = FL - pastFL;
	float dFR = FR - pastFR;
	float dBL = BL - pastBL;
	float dBR = BR - pastBR;
	
	float dL = (dFL + dBL) / 2.0;
	float dR = (dFR + dBR) / 2.0;
	
	float kTrans = 101.75 / ((1878.0 + 1861.0 + 1895.0 + 1841.0) / 2.0);
	float kRotat = 360.0 / ((3125.0 + 2524.0 + 2801.0 + 2527.0) / 2.0);
	float dx = kTrans * cos(theta / 180.0 * M_PI) * (dL + dR);
	float dy = kTrans * sin(theta / 180.0 * M_PI) * (dL + dR);
	float dt = kRotat * (dL - dR);
	
	x += dx;
	y += dy;
	theta += dt;
	
	pastFL = FL;
	pastFR = FR;
	pastBL = BL;
	pastBR = BR;
}
