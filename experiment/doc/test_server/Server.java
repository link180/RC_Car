import java.awt.EventQueue;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;

public class Server extends JFrame {

	private JPanel contentPane;
	private JTextField textField;
	private JButton Start;
	private JButton DoorOpen;	//DoorOpen
	private JButton Lock;
	JTextArea textArea;

	private ServerSocket socket;
	private Socket soc;
	private int Port;

	private Vector vc = new Vector();

	public static void main(String[] args)
	{

		Server frame = new Server();
		frame.setVisible(true);

	}

	public Server() {

		init();

	}

	private void init() {

		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 400, 400);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		contentPane.setLayout(null);


		JScrollPane js = new JScrollPane();

		textArea = new JTextArea();
		textArea.setColumns(20);
		textArea.setRows(5);
		js.setBounds(0, 0, 264, 254);
		contentPane.add(js);
		js.setViewportView(textArea);

		textField = new JTextField();
		textField.setBounds(110, 264, 154, 37);
		contentPane.add(textField);
		textField.setColumns(10);

		JLabel lblNewLabel = new JLabel("Port Number");
		lblNewLabel.setBounds(12, 264, 98, 37);
		contentPane.add(lblNewLabel);


	//DoorOpen button actionlistener/////////////////////////////////////////////////////////////////


		DoorOpen = new JButton("Door301");

		DoorOpen.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				if (textField.getText().equals("") || textField.getText().length()==0)// textField has no value
				{

					textField.setText("insert port number");
					textField.requestFocus();
				}

				else
				{
					try
					{

					Port = Integer.parseInt(textField.getText());

					server_start();

					}
					catch(Exception er)
					{

						textField.setText("insert number");
						textField.requestFocus();
					}
				}

			}
		});

		DoorOpen.setBounds(275, 10, 110, 37);
		contentPane.add(DoorOpen);


	//Lock button actionlistener/////////////////////////////////////////////////////////////////////

		Lock = new JButton("Lock");

		Lock.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				if (textField.getText().equals("") || textField.getText().length()==0)// textField has no value
				{

					textField.setText("insert port number");
					textField.requestFocus();
				}

				else
				{
					try
					{

					Port = Integer.parseInt(textField.getText());

					server_start();

					}
					catch(Exception er)
					{

						textField.setText("insert number");
						textField.requestFocus();
					}
				}

			}
		});

		Lock.setBounds(275, 70, 110, 37);
		contentPane.add(Lock);
	///////////////////////////////////////////////////////////////////////////////////////////////

	//Start button actionlistener//////////////////////////////////////////////////////////////////

		Start = new JButton("server start");

		Start.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {

				if (textField.getText().equals("") || textField.getText().length()==0)// textField has no value
				{

					textField.setText("insert port number");
					textField.requestFocus();
				}

				else
				{
					try
					{

					Port = Integer.parseInt(textField.getText());

					server_start();

					}
					catch(Exception er)
					{

						textField.setText("insert number");
						textField.requestFocus();
					}
				}

			}
		});

		Start.setBounds(0, 325, 264, 37);
		contentPane.add(Start);
	////////////////////////////////////////////////////////////////////////////////////////////////

		textArea.setEditable(false);

	}

	private void server_start() {

		try {
			socket = new ServerSocket(Port);
			Start.setText("server start");
			Start.setEnabled(false);
			textField.setEnabled(false);

			if(socket!=null)
			{
			Connection();
			}

		} catch (IOException e) {
			textArea.append("already socket used\n");

		}

	}

	private void Connection() {

		Thread th = new Thread(new Runnable() {

			@Override
			public void run() {
				while (true) {
					try {
						textArea.append("wating...\n");
						soc = socket.accept();
						textArea.append("user connect!!\n");

						UserInfo user = new UserInfo(soc, vc);

						vc.add(user);

						user.start();

					} catch (IOException e) {
						textArea.append("!!!! accept error... !!!!\n");
					}
				}
			}
		});

		th.start();
	}

	class UserInfo extends Thread {

		private InputStream is;
		private OutputStream os;
		private DataInputStream dis;
		private DataOutputStream dos;

		private Socket user_socket;
		private Vector user_vc;

		private String Nickname = "";

		public UserInfo(Socket soc, Vector vc)
		{

			this.user_socket = soc;
			this.user_vc = vc;

			User_network();

		}

		public void User_network() {
			try {
				is = user_socket.getInputStream();
				dis = new DataInputStream(is);
				os = user_socket.getOutputStream();
				dos = new DataOutputStream(os);

				Nickname = dis.readUTF();
				textArea.append("user ID " +Nickname+"\n");

				send_Message("connect!");

			} catch (Exception e) {
				textArea.append("stream error\n");
			}

		}

		public void InMessage(String str) {
			textArea.append("user massages : " + str+"\n");

			broad_cast(str);

		}

		public void broad_cast(String str) {
			for (int i = 0; i < user_vc.size(); i++) {
				UserInfo imsi = (UserInfo) user_vc.elementAt(i);
				imsi.send_Message(Nickname+" : "+str);

			}

		}

		public void send_Message(String str) {
			try {
				dos.writeUTF(str);
			}
			catch (IOException e) {
				textArea.append("message error\n");
			}
		}

		public void run()
		{

			while (true) {
				try {

					String msg = dis.readUTF();
					InMessage(msg);

				}
				catch (IOException e)
				{

					try {
						dos.close();
						dis.close();
						user_socket.close();
						vc.removeElement( this );
						textArea.append(vc.size() +" : user number\n");
						textArea.append("user unconnected\n");
						break;

					} catch (Exception ee) {

					}
				}

			}

		}

	}

}
