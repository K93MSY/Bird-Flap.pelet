# include <Siv3D.hpp> // OpenSiv3D v0.6.6
# include <math.h>
# include <numbers>
# include <iostream>

void repop(int pellet_x[80], int pellet_y[80], int pellet_size[80], int i) {
	pellet_x[i] = 480;
	pellet_y[i] = Random() * 540;
	if (pellet_y[i] > 480) {
		pellet_size[i] = 20;
	}
	else if (pellet_y[i] > 300) {
		pellet_size[i] = 12;
	}
	else {
		pellet_size[i] = 8;
	}
}

void repopv(int pellet_x[80], int pellet_y[80], int pellet_size[80], int i) {
	pellet_x[i] = 480;
	pellet_y[i] = Random() * 540;
}

void Main()
{
	Window::Resize(480, 640);
	Window::SetTitle(U"Bird Flap.pelet");
	// 背景の色を設定 | Set background color
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	//About FPS
	int FPS = 60; // 1秒間に1画面を書き換える回数
	Stopwatch sw;   //FPS60
	sw.start();

	// 通常のフォントを作成 | Create a new font
	const Font font_30{ 30 };
	const Font font_31{ 31 };
	const Font font_15{ 15 };
	// Read Audio
	const Audio audio_jump{ U"assets/jump06.mp3" };

	bool start = false;
	bool restart = false;
	bool network = false;
	bool senddata = false;
	bool onload = false;
	long long ticker = 0;
	double speed = 0;
	double fish_y = 301;
	double bird_deg = 0;
	int xx = 213;
	int score = 0;
	int bestscore = 0;
	int time = 60;
	int get60f = 0;
	String bestplayer;

	bool hitbox = false;

	int pellet_x[80];
	int pellet_y[80];
	int pellet_size[80];
	for (int i = 0; i < 80; i++) {
		pellet_x[i] = Random() * 480;
		pellet_y[i] = Random() * 540;
		if (pellet_y[i] > 480) {
			pellet_size[i] = 20;
		}
		else if(pellet_y[i] > 300) {
			pellet_size[i] = 12;
		}
		else {
			pellet_size[i] = 8;
		}
	}

	const int virusamount = 10;
	int virus_x[virusamount];
	int virus_y[virusamount];
	int virus_size[virusamount];
	for (int i = 0; i < virusamount; i++) {
		virus_x[i] = Random() * 480;
		virus_y[i] = Random() * 540;
		virus_size[i] = 16;
	}

	TextEditState player;
	player.text = U"名無し";

	const Texture background_img{ U"assets/background.png" };
	const Texture bird{ U"assets/bird.png" };
	const Texture restart_img{ U"assets/restart.png" };
	const Texture ground{ U"assets/ground.png" };
	const Texture coin{ U"assets/coin.svg" };
	const Texture score_img{ U"assets/score.png" };
	const Texture wifi_on{ 0xf1eb_icon, 20 };

	//Online Check
	const HashTable<String, String> headers = { {U"Authorization", U"get" } };
	const FilePath saveFilePath = U"auth_result.json";
	if (const auto response = SimpleHTTP::Get(U"https://siv3d.up.railway.app/get", headers, saveFilePath))
	{

		if (response.isOK())
		{
			network = true;
		}
	}
	else
	{
		network = false;
	}

	while (System::Update())
	{
		ticker++;
		Rect{ 0, -64, 480, 640 }(background_img).draw();
		if (ticker % 3 == 0) {
			for (int i = 0; i < 50; i++) {
				Rect{ i * 18, 640 - 64, 18, 64 }(ground).draw();
			}
		}
		else if (ticker % 3 == 1) {
			for (int i = 0; i < 50; i++) {
				Rect{ i * 18 - 6, 640 - 64, 18, 64 }(ground).draw();
			}
		}
		else {
			for (int i = 0; i < 50; i++) {
				Rect{ i * 18 - 12, 640 - 64, 18, 64 }(ground).draw();
			}
		}

		//Line{ 239.5, 0, 239.5, 640 }.draw(1, Palette::Yellow);

		if (start) {
			if (get60f == 60) {
				get60f = 0;
				time--;
				if (time == 0) {
					bird_deg = 0;
					restart = true;
					start = false;
				}
			}
			//About fps++
			get60f++;
			//About Score
			for (int i = 0; i < 80; i++) {
				pellet_x[i]-=2;

				if (pellet_x[i] < -10) {/*
					pellet_x[i] = 480;
					pellet_y[i] = Random() * 540;
					if (pellet_y[i] > 480) {
						pellet_size[i] = 20;
					}
					else if (pellet_y[i] > 300) {
						pellet_size[i] = 12;
					}
					else {
						pellet_size[i] = 8;
					}*/
					repop(pellet_x, pellet_y, pellet_size, i);
				}
				Rect{ pellet_x[i] + pellet_size[i] / 2, pellet_y[i] - pellet_size[i] / 2, pellet_size[i]}.draw(Palette::Yellow);
				if (pellet_x[i]-pellet_size[i]/2 > xx && pellet_x[i] + pellet_size[i] / 2 < xx + 54 && pellet_y[i] - pellet_size[i] / 2 >fish_y && pellet_y[i] + pellet_size[i] / 2 <fish_y + 39) {
					score += pellet_size[i]*virusamount*0.2;
					repop(pellet_x, pellet_y, pellet_size, i);
				}
			}

			// About virus
			for (int i = 0; i < virusamount; i++) {
				virus_x[i] -= 2;

				if (virus_x[i] < -10) {
					repopv(virus_x, virus_y, virus_size, i);
				}
				Rect{ virus_x[i] + virus_size[i] / 2, virus_y[i] - virus_size[i] / 2, virus_size[i] }.draw(Palette::Green);
				if (virus_x[i] - virus_size[i] / 2 > xx && virus_x[i] + virus_size[i] / 2 < xx + 54 && virus_y[i] - virus_size[i] / 2 > fish_y && virus_y[i] + virus_size[i] / 2 < fish_y + 39) {
					bird_deg = 0;
					restart = true;
					start = false;
				}
			}

			font_15(U"Score").drawAt(Vec2{ 40, 20 }, Palette::Black);
			font_15(score).drawAt(Vec2{ 80,20 }, Palette::Black);

			font_15(U"残り時間").drawAt(Vec2{ 350, 20 }, Palette::Black);
			font_15(time).drawAt(Vec2{ 400,20 }, Palette::Black);

			//About Chara

			if (speed < 13) {
				speed += 0.25;
				bird_deg += 2;
			}
			fish_y += speed;
			int yy = fish_y;
			Rect roundRect{ xx, yy, 54, 39 };
			roundRect.draw(HSV{ 0,0,100,0 });
			if (ticker % 6 == 0 || ticker % 6 == 1) {
				roundRect.rotated(bird_deg * (std::numbers::pi / 180))(bird(0, 0, 90, 65)).draw();
			}
			else if (ticker % 6 == 2 || ticker % 6 == 3) {
				roundRect.rotated(bird_deg * (std::numbers::pi / 180))(bird(90, 0, 90, 65)).draw();
			}
			else {
				roundRect.rotated(bird_deg * (std::numbers::pi / 180))(bird(180, 0, 90, 65)).draw();
			}

			//Print (bird_deg);
			if (MouseL.down())
			{
				audio_jump.playOneShot();
				bird_deg = -55;
				speed = -6;
			}
			if (yy > 535) {
				bird_deg = 0;
				restart = true;
				start = false;
			}
			if (KeyA.pressed() && xx > 0 || KeyLeft.pressed() && xx > 0) {
				//Print << U"Moving Left";
				xx-=3;
			}
			if (KeyD.pressed() && xx < 426 || KeyRight.pressed() && xx < 426) {
				//Print << U"Moving Right";
				xx+=3;
			}

			//hitbox
			if (hitbox) {
				Rect{ xx, yy, 54 ,39 }.draw(ColorF{ 0.0, 0.0, 0.0, 0.0 }).drawFrame(1, Palette::Red);
			}
		}
		else {
			if (restart) {
				Rect{ 150,120, 180, 250 }(score_img).draw();
				JSON onlinedata = JSON::Load(U"api.json");
				if (onload == false) {
					onload = true;
					const HashTable<String, String> headers = { {U"Authorization", U"get" } };
					const FilePath saveFilePath = U"api.json";
					if (const auto response = SimpleHTTP::Get(U"https://siv3d.up.railway.app/api", headers, saveFilePath))
					{

						if (response.isOK())
						{
							onlinedata = JSON::Load(U"api.json");
							network = true;
						}
					}
					else
					{
						Print << U"Network error";
						network = false;
					}

				}
				font_30(onlinedata[U"score"].get<int32>()).drawAt(Vec2{ 240,305 }, Palette::Black);
				font_15(onlinedata[U"body"].getString()).drawAt(Vec2{ 280,330 }, Palette::Black);
				font_31(score).drawAt(Vec2{ 240,225 }, Palette::Black);
				if (senddata == false) {
					SimpleGUI::TextBox(player, Vec2{ 150, 400 });
					if (SimpleGUI::Button(U"リザルトを投稿", Vec2{ 160, 450 })) {
						senddata = true;
						font_31(U"サーバーにデータを送信中").drawAt(Vec2{ 240, 320 }, Palette::Black);
						const URL url = U"https://siv3d.up.railway.app/post";
						//const URL url = U"http://localhost:443/post";
						const HashTable<String, String> headers = { { U"Content-Type", U"application/json"}};
						const std::string data = JSON
						{
							{ U"body", player.text},
							{ U"score", score},
							{ U"date", DateTime::Now().format() },
						}.formatUTF8();
						const FilePath saveFilePath = U"post_result.json";

						if (auto response = SimpleHTTP::Post(url, headers, data.data(), data.size(), saveFilePath))
						{/*
							Console << U"------";
							Console << response.getStatusLine().rtrimmed();
							Console << U"status code: " << FromEnum(response.getStatusCode());
							Console << U"------";
							Console << response.getHeader().rtrimmed();
							Console << U"------";
							if (response.isOK())
							{
								Print << TextReader{ saveFilePath }.readAll();
							}*/

						}
						else
						{
							//Print << U"Failed.";
						}
					}
				}
				if (SimpleGUI::Button(U"Back Home", Vec2{ 170, 500 })) {
					restart = false;
				}
			}
			else {
				Rect roundRect{ 213, 301, 54, 39 };
				font_31(U"Bird Flap.pelet").drawAt(Vec2{ 240, 250 }, Palette::Black);
				font_31(U"Bird Flap.pelet").drawAt(Vec2{ 237, 248 }, Palette::White);
				if (SimpleGUI::Button(U"Start", Vec2{ 195, 350 }))
				{
					get60f = 0;
					onload = false;
					senddata = false;
					speed = 0;
					score = 0;
					fish_y = 301;
					start = true;
					bird_deg = 0;
					xx = 213;
				}

				SimpleGUI::CheckBox(hitbox, U"当たり判定を表示する", Vec2{ 10, 10 });

				if (network) {
					wifi_on.drawAt(15, 620, ColorF{ 0.25 });
				}
				else {
					font_15(U"現在はオフラインです").drawAt(Vec2{ 100, 620 }, Palette::Black);
				}

				roundRect.draw(HSV{ 0,0,100,0 });
				if (ticker % 6 == 0 || ticker % 6 == 1) {
					roundRect.rotated(bird_deg * (std::numbers::pi / 180))(bird(0, 0, 90, 65)).draw();
				}
				else if (ticker % 6 == 2 || ticker % 6 == 3) {
					roundRect.rotated(bird_deg * (std::numbers::pi / 180))(bird(90, 0, 90, 65)).draw();
				}
				else {
					roundRect.rotated(bird_deg * (std::numbers::pi / 180))(bird(180, 0, 90, 65)).draw();
				}
			}
		}

		while (sw.msF() < 1000.0 / FPS);    //1/60秒経過するまでループ
		sw.restart();
	}
}

