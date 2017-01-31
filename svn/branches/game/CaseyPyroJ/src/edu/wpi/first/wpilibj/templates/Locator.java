/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.wpi.first.wpilibj.templates;

/**
 *
 * @author Currey-WilsonC
 */
public class Locator implements Part {

    private double x;
    private double y;
    private double theta;
    private double pastFL;
    private double pastFR;
    private double pastBL;
    private double pastBR;

    public Locator()
    {
        x = y = theta = pastFL = pastFR = pastBL = pastBR = 0;
    }

    public void runDiagnostics(PyroMain main){}

    public void initAutonomous(PyroMain main)
    {
    }

    public void initOperator(PyroMain main)
    {
    }

    public void runAutonomous(PyroMain main)
    {
        update(main);
    }

    public void runOperator(PyroMain main)
    {
        update(main);
    }

    private void update(PyroMain main)
    {
        // DOES NOT TAKE STRAFING INTO ACCOUNT
	/*double FL = main.hal.getFLEncoder();
	double FR = main.hal.getFREncoder();
	double BL = main.hal.getBLEncoder();
	double BR = main.hal.getBREncoder();

	double dFL = FL - pastFL;
	double dFR = FR - pastFR;
	double dBL = BL - pastBL;
	double dBR = BR - pastBR;

	double dL = (dFL + dBL) / 2.0;
	double dR = (dFR + dBR) / 2.0;

	double kTrans = 101.75 / ((1878.0 + 1861.0 + 1895.0 + 1841.0) / 2.0);
	double kRotat = 360.0 / ((3125.0 + 2524.0 + 2801.0 + 2527.0) / 2.0);
	double dx = kTrans * Math.cos(theta / 180.0 * Math.PI) * (dL + dR);
	double dy = kTrans * Math.sin(theta / 180.0 * Math.PI) * (dL + dR);
	double dt = kRotat * (dL - dR);

	x += dx;
	y += dy;
	theta += dt;

	pastFL = FL;
	pastFR = FR;
	pastBL = BL;
	pastBR = BR;*/
    }

    public double getX() { return x; }
    public double getY() { return y; }
    public double getTheta() { return theta; }

    public void reset(PyroMain main) {
        x = y = theta = 0;
        /*pastFL = main.hal.getFLEncoder();
        pastFR = main.hal.getFREncoder();
        pastBL = main.hal.getBLEncoder();
        pastBR = main.hal.getBREncoder();*/
    }
}
