/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.wpi.first.wpilibj.templates;

/**
 *
 * @author Currey-WilsonC
 */
public class Diagnostics implements Part
{
    int tmr;

    public Diagnostics() {}

    public void initAutonomous(PyroMain main){}
    public void initOperator(PyroMain main){}
    public void runAutonomous(PyroMain main){}
    public void runOperator(PyroMain main){}

    public void runDiagnostics(PyroMain main)
    {
        tmr = 0;
	if (tmr++ > 1000)
	{
		tmr = 0;
		//printf("Diagnostics::runDiagnostics()\n");
	}

	main.hal.stopMotors();

	//main.locator.reset(main);

	if (main.hal.rStick.getRawButton(3))
		main.hal.setSuckerPower(main.hal.rStick.getY());

	main.hal.setKickerLatch(main.hal.rStick.getTrigger());

	main.hal.setWinchShootPower(0.f);

	if (main.hal.lStick.getRawButton(3))
		main.hal.setWinchShootPower(main.hal.lStick.getY());

	main.hal.setKickerForwardValue(main.hal.hStick.getTrigger());

	if (main.hal.hStick.getRawButton(6))
		main.hal.setFLPower(main.hal.hStick.getThrottle());
	if (main.hal.hStick.getRawButton(7))
		main.hal.setBLPower(main.hal.hStick.getThrottle());
	if (main.hal.hStick.getRawButton(11))
		main.hal.setFRPower(main.hal.hStick.getThrottle());
	if (main.hal.hStick.getRawButton(10))
		main.hal.setBRPower(main.hal.hStick.getThrottle());
    }
}
