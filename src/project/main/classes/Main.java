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

public class Main {
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
		
		try {
			Controllers.create();
		}catch(Exception e){
			e.printStackTrace();
		}
		
		Controllers.poll();
		
		
		
		for(int i=0; i<Controllers.getControllerCount();i++){
			controller =  Controllers.getController(i);
			System.out.println(controller.getName()+ " : " + controller.getIndex());
		}
		
		controller = Controllers.getController(7);

		
		
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
				if(btnNewButton.getText().equals("LED ON")) {
					data = "on";
					btnNewButton.setText("LED OFF");
				}else{
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

				while (sending) {
//					System.out.println("data is: " + data);
					 
					System.out.println("X : "+controller.getXAxisValue());
					System.out.println("Y : "+controller.getYAxisValue());
					System.out.println("Z : "+controller.getZAxisValue());
					
					textField.setText("X : "+controller.getXAxisValue()+ "   "+"Y : "+controller.getYAxisValue()+"   "+"Z : "+controller.getZAxisValue());
					
					controller.poll();
					
					if(data != null){
					output.print(data.toString());
					output.flush();
					try {
						Thread.sleep(100);
					} catch (Exception e) {
					}
					System.out.println("Successfully Sending!");
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
