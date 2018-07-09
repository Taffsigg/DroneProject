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

import com.fazecast.jSerialComm.SerialPort;

public class Main {
	String s;
	private JComboBox<String> comboBox;
	private JFrame frame;
	private JPanel panel;
	static SerialPort chosenPort;
	private boolean sending;
	Thread thread2 = null;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					Main window = new Main();
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
	public Main() {
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

				String data = null;

				sending = true;

				while (sending) {
					output.print(data);
					output.flush();
					try {
						Thread.sleep(100);
					} catch (Exception e) {
					}
					System.out.println("Successfully Sending!");
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
						System.out.println(number);
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
