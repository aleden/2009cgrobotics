/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.wpi.first.wpilibj.templates;

import edu.wpi.first.wpilibj.*;

/**
 *
 * @author Currey-WilsonC
 */
public class Sucker implements Part
{

    //enum SuckState { suckIn, suckOut, suckStop };

    private int state;
    Timer timer;
    double currentVoltage; // not actually in volts; [-1,1]

    public Sucker()
    {
        timer = new Timer();
        currentVoltage = 0;
        state = 2;
    }

    public void runDiagnostics(PyroMain main){}
    
    public void initAutonomous(PyroMain main)
    {
        timer.start();
        timer.reset();
    }
    public void initOperator(PyroMain main)
    {
        timer.start();
        timer.reset();
    }
    public void runAutonomous(PyroMain main)
    {
        updateSucker(main);
    }
    public void runOperator(PyroMain main)
    {
        updateSucker(main);
    }

    public double getVoltage() { return currentVoltage; }
    public void suck() {
        state = 0;
        if (currentVoltage >= 0) currentVoltage = -1;
    }
    public void blow() { state = 1; }
    public void stop() { state = 2; }

    private void updateSucker(PyroMain main)
    {
        double tdelta = timer.get();
	timer.reset();

	double setpoint = 10; //(main.hal.rStick.GetThrottle() + 1) / 2.0 * 5.0 + 5.0;
	//static int foo = 10;
	//if (foo-- == 0) {
	//	printf("%f\n",setpoint);
	//	foo = 10;
	//}
	double kP = 0.25; // needs to be tuned
	double realpoint;
        double delta;

	switch (state) {
            case 2:
                    currentVoltage = 0;
                    break;
            case 1:
                    currentVoltage = 1;
                    break;
            case 0:
                    if (currentVoltage > -0.25) currentVoltage = -0.25;
                    realpoint = main.hal.getSuckerCurrent();
                    delta = setpoint - realpoint;
                    currentVoltage -= delta * kP * tdelta;
	}

	if (currentVoltage > 1.0) currentVoltage = 1.0;
	if (currentVoltage < -1.0) currentVoltage = -1.0;
	main.hal.setSuckerPower(currentVoltage);
    }
}
