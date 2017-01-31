/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.templates;


import edu.wpi.first.wpilibj.*;
import java.util.*;

/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the SimpleRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class PyroMain extends IterativeRobot {
    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    Vector parts;
    Control control;
    Script script;
    Locator locator;
    Driver driver;
    Hanger hanger;
    Sucker sucker;
    //Camera camera;
    Diagnostics diagnostics;
    HAL hal;
    boolean isDiagnosticsMode;
	int test;

    public boolean isDiagnosticsModeEnabled() { return isDiagnosticsMode; }

    public void setDiagnosticsModeEnabled(boolean enabled)
    {
            isDiagnosticsMode = enabled;
    }

    public PyroMain() {
        isDiagnosticsMode = false;
        getWatchdog().setEnabled(false);
        parts = new Vector();
        parts.addElement(control = new Control());
        //parts.addElement(script = new Script());
        //parts.addElement(locator = new Locator());
        parts.addElement(driver = new Driver());
        //parts.addElement(hanger = new Hanger());
        parts.addElement(sucker = new Sucker());
        //parts[6] = new Camera();
        parts.addElement(diagnostics = new Diagnostics());
        parts.addElement(hal = new HAL());
    }

    /*public void disabled()
    {
        getWatchdog().setEnabled(false);
        for (;;) {
            control.runDisabled(this);
        }
    }

    public void autonomous() {
        //init auto mode
        getWatchdog().setEnabled(false);
        for (int i=0; i<parts.size(); i++){
            parts[i].initAutonomous(this);
        }
        for (;;) {
            for (int i=0; i<parts.size(); i++){
                parts[i].runAutonomous(this);
            }

            Timer.delay(.02);
        }
    }

    /**
     * This function is called once each time the robot enters operator control.
     
    public void operatorControl() {
        //init operator mode
        getWatchdog().setEnabled(false);
        for (int i=0; i<parts.size(); i++){
            parts[i].initOperator(this);
        }
        for (;;) {
            if (isDiagnosticsMode) {
                for (int i=0; i<parts.size(); i++){
                    parts[i].runDiagnostics(this);
                }
            }
            else {
                for (int i=0; i<parts.size(); i++){
                    parts[i].runOperator(this);
                }
            }

            Timer.delay(.02);
        }
    }
     * */

    public void autonomousInit()
    {
        getWatchdog().setEnabled(false);
        for (int i=0; i<parts.size(); i++){
            ((Part)parts.elementAt(i)).initAutonomous(this);
        }
    }

    public void autonomousPeriodic()
    {
        for (int i=0; i<parts.size(); i++){
            ((Part)parts.elementAt(i)).runAutonomous(this);
        }
    }

    public void teleopInit()
    {
        getWatchdog().setEnabled(false);
        for (int i=0; i<parts.size(); i++){
            ((Part)parts.elementAt(i)).initOperator(this);
        }
    }

    public void teleopPeriodic()
    {
        if (isDiagnosticsMode) {
            for (int i=0; i<parts.size(); i++){
                ((Part)parts.elementAt(i)).runDiagnostics(this);
            }
        }
        else {
            for (int i=0; i<parts.size(); i++){
                ((Part)parts.elementAt(i)).runOperator(this);
            }
        }
    }

    public void disabledInit()
    {
        getWatchdog().setEnabled(false);
    }

    public void disabledPeriodic()
    {
        control.runDisabled(this);
    }
}
