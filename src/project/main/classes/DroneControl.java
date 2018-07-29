package project.main.classes;

import java.awt.EventQueue;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;

import com.fazecast.jSerialComm.SerialPort;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.JToggleButton;

public class DroneControl {
	String s;
	private JComboBox<String> comboBox;
	private JFrame frame;
	private JPanel panel;
	static SerialPort chosenPort;
	private boolean sending;
	Thread thread2 = null;
	String data;
	private JButton btnNewButton;
	static Controller controller;
	private JTextField textField;
	JToggleButton tglbtnNewToggleButton;
	JSlider slider = new JSlider();
	JSlider slider_1 = new JSlider();
	JSlider slider_2 = new JSlider();
	JSlider slider_3 = new JSlider();

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					DroneControl window = new DroneControl();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public DroneControl() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame();
		panel = new JPanel();
		frame.setBounds(100, 100, 1280, 720);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setTitle("Drone Alpha v0.1");
		frame.setResizable(false);
		panel.setLayout(null);
		frame.setContentPane(panel);
		comboBox = new JComboBox<String>();
		comboBox.setBounds(10, 11, 297, 20);
		panel.add(comboBox);

		try {
			Controllers.create();
		} catch (Exception e) {
			e.printStackTrace();
		}

		Controllers.poll();

		Controller cont = null;

		for (int i = 0; i < Controllers.getControllerCount(); i++) {
			controller = Controllers.getController(i);
			System.out.println(controller.getName() + " : " + controller.getIndex());
			if (controller.getName().equals("Mad Catz V.1 Stick")) {
				cont = Controllers.getController(i);
				for (int i1 = 0; i1 < cont.getButtonCount(); i1++) {
					System.out.println(cont.getButtonName(i1) + ": " + cont.isButtonPressed(i1));

				}
			}
		}

		controller = cont;

		SerialPort[] portNames = SerialPort.getCommPorts();
		for (int i = 0; i < portNames.length; i++)
			comboBox.addItem(portNames[i].getSystemPortName());

		JButton btnConnect = new JButton("Connect");
		btnConnect.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (btnConnect.getText().equals("Connect")) {
					if (!(comboBox.getItemCount() == 0)) {
						// attempt to connect to the serial port
						chosenPort = SerialPort.getCommPort(comboBox.getSelectedItem().toString());
						chosenPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);
						if (chosenPort.openPort()) {
							btnConnect.setText("Disconnect");
							comboBox.setEnabled(false);
						}

						// create a new thread that listens for incoming text
						// and populates the graph

						reciveInputfromSerial(true);
						sendInputToSerial(true);

					} else {
						JOptionPane.showMessageDialog(null, "No ports aviable!");
					}
				} else {
					// disconnect from the serial port
					chosenPort.closePort();
					comboBox.setEnabled(true);
					btnConnect.setText("Connect");
					reciveInputfromSerial(false);
					sendInputToSerial(false);
				}
			}
		});
		btnConnect.setBounds(10, 42, 89, 23);
		panel.add(btnConnect);

		btnNewButton = new JButton("LED ON");
		btnNewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (btnNewButton.getText().equals("LED ON")) {
					data = "on";
					btnNewButton.setText("LED OFF");
				} else {
					data = "off";
					btnNewButton.setText("LED ON");
				}
			}
		});
		btnNewButton.setBounds(10, 80, 297, 23);
		panel.add(btnNewButton);

		textField = new JTextField();
		textField.setBounds(10, 114, 583, 20);
		panel.add(textField);
		textField.setColumns(10);

		JLabel lblController = new JLabel("Controller :");
		lblController.setBounds(10, 162, 123, 14);
		panel.add(lblController);

		slider.setValue(100);
		slider.setMaximum(200);
		slider.setBounds(86, 214, 200, 26);
		panel.add(slider);

		slider_1.setValue(100);
		slider_1.setMaximum(200);
		slider_1.setBounds(86, 262, 200, 26);
		panel.add(slider_1);

		slider_2.setValue(100);
		slider_2.setMaximum(200);
		slider_2.setBounds(86, 315, 200, 26);
		panel.add(slider_2);

		slider_3.setValue(100);
		slider_3.setMaximum(200);
		slider_3.setBounds(86, 366, 200, 26);
		panel.add(slider_3);

		JLabel lblThrottle = new JLabel("Throttle: ");
		lblThrottle.setBounds(10, 214, 66, 14);
		panel.add(lblThrottle);

		JLabel lblYaw = new JLabel("Yaw: ");
		lblYaw.setBounds(10, 262, 66, 14);
		panel.add(lblYaw);

		JLabel lblRoll = new JLabel("Roll: ");
		lblRoll.setBounds(10, 315, 66, 14);
		panel.add(lblRoll);

		JLabel lblPitch = new JLabel("Pitch: ");
		lblPitch.setBounds(10, 366, 66, 14);
		panel.add(lblPitch);

		tglbtnNewToggleButton = new JToggleButton("Start");
		tglbtnNewToggleButton.setBounds(86, 416, 200, 23);
		panel.add(tglbtnNewToggleButton);
		tglbtnNewToggleButton.setEnabled(false);

	}

	public void sendInputToSerial(boolean start) {
		Thread thread = new Thread() {
			@Override
			public void run() {
				// wait after connecting, so the bootloader can finish
				try {
					Thread.sleep(100);
				} catch (Exception e) {
				}

				// enter an infinite loop that sends text to the arduino
				PrintWriter output = new PrintWriter(chosenPort.getOutputStream());

				data = null;

				sending = true;

				boolean startdrone = false;
				String sd = "0";

				while (sending) {
					// System.out.println("data is: " + data);

					if (controller.isButtonPressed(6)) {
						System.out.println("pressed");
						if (!startdrone) {
							startdrone = true;
							sd = "1";
							tglbtnNewToggleButton.setSelected(true);
						} else {
							startdrone = false;
							sd = "0";
							tglbtnNewToggleButton.setSelected(false);
						}

					}

					String x = String.format("%.2f", controller.getXAxisValue() + 1);
					String y = String.format("%.2f", controller.getYAxisValue() + 1);
					String z = String.format("%.2f", -((controller.getZAxisValue() + 1) - 2));
					String rz = String.format("%.2f", controller.getRZAxisValue() + 1);

					slider_3.setValue(Math.round((controller.getYAxisValue() + 1) * 100));
					slider_2.setValue(Math.round((controller.getXAxisValue() + 1) * 100));
					slider_1.setValue(Math.round((controller.getRZAxisValue() + 1) * 100));
					if (-((controller.getZAxisValue() + 1) * 100 - 200) != -0.0) {
						slider.setValue(Math.round(-((controller.getZAxisValue() + 1) * 100 - 200)));
					} else {
						slider.setValue(0);
						z = "0.0";
					}

					data = x + " " + y + " " + z + " " + rz;
					textField.setText("X : " + x + "   " + "Y : " + y + "   " + "Z : " + z + "  RZ : " + rz
							+ " startdrone : " + sd);

					controller.poll();

					if (data != null) {
						output.print(data.toString());
						output.flush();
						try {
							Thread.sleep(100);
						} catch (Exception e) {
						}
						System.out.println("Successfully Sending! " + data);
					}
				}
			}
		};
		if (start == true) {
			thread.start();
		} else {
			sending = false;
			thread.interrupt();
			System.out.println("send thread is alive:" + thread.isAlive());
		}
	}

	public void reciveInputfromSerial(boolean start) {
		thread2 = new Thread() {
			@Override
			public void run() {
				Scanner scanner = new Scanner(chosenPort.getInputStream());
				try {
					System.out.println(chosenPort.getInputStream().read());
					System.out.println("recive thread is alive:" + thread2.isAlive());
				} catch (IOException e1) {
					e1.printStackTrace();
				}
				while (scanner.hasNextLine()) {
					try {
						String line = scanner.nextLine();
						int number = Integer.parseInt(line);
						System.out.println("number is: " + number);
						System.out.println("Successfully reciving!");
					} catch (Exception e) {
					}
				}
				scanner.close();
			}
		};
		if (start == true) {
			thread2.start();
		} else {
			if (thread2.isAlive()) {
				thread2.interrupt();
				;
			}
		}
	}
}