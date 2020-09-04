#include <Siv3D.hpp> // OpenSiv3D v0.4.0

void QBox(int x, int y, Point Ratio);
void PowerBox(int x, int y, double Power, Point Ratio);
void MakeQuestion(int& FirstTerm, int& SecondTerm, String& OperatorSymbol, int& AnswerTerm, bool& MistakeFlag);
void PowerCal(double& Power, Point Momentum);
void OpenQuest(int x, int y, int FirstTerm, int SecondTerm, String OperatorSymbol, int AnswerTerm, double Power, Font font);
void PowerCable(int x, int yTop, int yBottom, double Power, Array<int>& yElect,Point Ratio);

void Main()
{

	Window::SetTitle(U"フリフリ！答え合わせ！");
	Window::SetFullscreen(true);
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	int GAMEMODE = 0;
	int hiscore[3] = {0, 0, 0};
	double hirate;
	String line;
	const Font font60(60), font80(80), font100(100);
	int first_term, second_term, answer_term;
	String operator_symbol;
	bool mistake_flag;
	double power;
	int score, false_score, true_score;
	double rate;
	Point momentum;
	Array<int> elect_point;
	Timer timer;
	const Audio count(U"sound/Countdown.mp3");
	const Audio correct(U"sound/Correct_Answer.mp3");
	const Audio wrong(U"sound/Wrong_Buzzer.mp3");
	const Audio result(U"sound/Results.mp3");
	TextReader reader(U"Hiscore.txt");
	TextWriter writer;
	int WINDOW_CENTER_X = Window::ClientWidth() / 2;
	int WINDOW_ONEFIVE_X = Window::ClientWidth() / 5;
	int WINDOW_FOURFIVE_X = WINDOW_ONEFIVE_X * 4;
	int WINDOW_CENTER_Y = Window::ClientHeight() / 2;
	int WINDOW_ONEFOUR_Y = Window::ClientHeight() / 4;
	int WINDOW_ONEFIVE_Y = Window::ClientHeight() / 5;
	int WINDOW_FOURFIVE_Y = WINDOW_ONEFIVE_Y * 4;
	int WINDOW_ELEVENTWELVE_Y = Window::ClientHeight() / 12 * 11;
	Point WINDOW_RATIO(Window::ClientWidth() / 960, Window::ClientHeight() / 540);

	//ハイスコア読み込み
	for (int i = 0; reader.readLine(line); ++i)hiscore[i] = Parse<int32>(line);
	
	hirate = double(double(hiscore[1]) / double(hiscore[1] + hiscore[2])) * 100.0;
	if (isnan(hirate)) hirate = 0;
	reader.close();


	while (System::Update())
	{

		switch (GAMEMODE)
		{
		case 0:
			//タイトル
			Rect(WINDOW_CENTER_X - WINDOW_RATIO.x * 600, WINDOW_ONEFOUR_Y - 150 * WINDOW_RATIO.y, 1200 * WINDOW_RATIO.x, 300 * WINDOW_RATIO.y).drawFrame(10.0, Palette::Black).draw(Palette::White);
			Shape2D::Cross(100, 20, Vec2(WINDOW_ONEFIVE_X, WINDOW_ONEFOUR_Y)).draw(Palette::Royalblue);
			Circle(WINDOW_FOURFIVE_X, WINDOW_ONEFOUR_Y, 100).drawFrame(20.0, 0.0, Palette::Red);
			font100(U"フリフリ！答え合わせ！").drawAt(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y, Palette::Black);
			font60(U"クリックしてゲームスタート！").drawAt(WINDOW_CENTER_X, WINDOW_CENTER_Y, Palette::Black);
			font60(U"ハイスコア: ", hiscore[0], U"\t正解率: ", U"{:.1f}"_fmt(hirate), U"%").drawAt(WINDOW_CENTER_X, WINDOW_ELEVENTWELVE_Y, Palette::Black);

			if (MouseL.down() || MouseR.down())GAMEMODE = 1;

			break;

		case 1:
		//初期化
			GAMEMODE = 2;
			power = 0.0;
			score = 0;
			false_score = 0;
			true_score = 0;
			MakeQuestion(first_term, second_term, operator_symbol, answer_term, mistake_flag);
			timer.set(6s);
			timer.start();
			count.play();
			break;

		case 2:
		//開始カウントダウン

			font80(U"ゲーム開始まで").drawAt(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y, Palette::Black);
			font100(timer.s()).drawAt(WINDOW_CENTER_X, WINDOW_CENTER_Y, Palette::Red);
			font60(U"ゲームが始まったらマウスを振ろう！").drawAt(WINDOW_CENTER_X, WINDOW_FOURFIVE_Y, Palette::Black);

			if (timer.reachedZero())
			{
				GAMEMODE = 3;
				timer.set(120s);
				timer.start();
			}

			break;

		case 3:
		//ゲーム
		{
				//表示部
				QBox(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y, WINDOW_RATIO);
				PowerBox(WINDOW_CENTER_X, WINDOW_FOURFIVE_Y, power, WINDOW_RATIO);
				PowerCable(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y, WINDOW_FOURFIVE_Y, power, elect_point, WINDOW_RATIO);
				OpenQuest(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y, first_term, second_term, operator_symbol, answer_term, power, font80);
				Shape2D::Cross(100, 20, Vec2(WINDOW_ONEFIVE_X, WINDOW_CENTER_Y)).draw(Palette::Royalblue);
				Circle(WINDOW_FOURFIVE_X, WINDOW_CENTER_Y, 100).drawFrame(20.0, 0.0, Palette::Red);
				font60(U"スコア: ", score).draw(0, 0, Palette::Black);
				font60(U"残り時間 : ", timer.s(), U"秒").draw(WINDOW_CENTER_X, 0, Palette::Black);
				font60(U"不正解: ", false_score).drawAt(WINDOW_ONEFIVE_X, WINDOW_ELEVENTWELVE_Y, Palette::Black);
				font60(U"正解: ", true_score).drawAt(WINDOW_FOURFIVE_X, WINDOW_ELEVENTWELVE_Y, Palette::Black);
				font100(U"左").drawAt(WINDOW_ONEFIVE_X, WINDOW_CENTER_Y, Palette::Black);
				font100(U"右").drawAt(WINDOW_FOURFIVE_X, WINDOW_CENTER_Y, Palette::Black);

				//計算生成部
				//回答判定			
				if (MouseL.down())//間違い
				{
					if (mistake_flag)//正解
					{
						score++;
						true_score++;
						Circle(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y, 100).drawFrame(20.0, 0.0, Palette::Red);
						correct.playOneShot(0.5);
					}

					else//不正解
					{
						power -= 50;
						false_score++;
						Shape2D::Cross(100, 20, Vec2(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y)).draw(Palette::Royalblue);
						wrong.playOneShot(0.5);
					}

					MakeQuestion(first_term, second_term, operator_symbol, answer_term, mistake_flag);
				}
				else if (MouseR.down())//合う
				{
					if (mistake_flag)//不正解
					{
						power -= 50;
						false_score++;
						Shape2D::Cross(100, 20, Vec2(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y)).draw(Palette::Royalblue);
						wrong.playOneShot(0.5);
					}

					else//正解
					{
						score++;
						true_score++;
						Circle(WINDOW_CENTER_X, WINDOW_ONEFOUR_Y, 100).drawFrame(20.0, 0.0, Palette::Red);
						correct.playOneShot(0.5);
					}
					MakeQuestion(first_term, second_term, operator_symbol, answer_term, mistake_flag);
				}


				//移動量からpowerへ
				momentum = Cursor::Delta();
				PowerCal(power, momentum);

				if (timer.reachedZero())
				{
					rate = double(double (true_score) / double(true_score + false_score)) * 100.0;
					if (isnan(rate)) rate = 0;
					GAMEMODE = 4;
					result.play();
				}
			}

			break;

		case 4:
			//リザルト
			font100(U"結果発表").drawAt(WINDOW_CENTER_X, WINDOW_ONEFIVE_Y, Palette::Black);
			font80(U"正解\t", true_score, U"回\n不正解\t", false_score, U"回\n正解率\t", U"{:.1f}"_fmt(rate), U"%").drawAt(WINDOW_CENTER_X, WINDOW_CENTER_Y, Palette::Black);
			font100(U"スコア\t", score).drawAt(WINDOW_CENTER_X, WINDOW_FOURFIVE_Y, Palette::Black);
			font60(U"クリックでタイトルに戻る").drawAt(WINDOW_CENTER_X, WINDOW_ELEVENTWELVE_Y, Palette::Black);
			if (MouseL.down() || MouseR.down())
			{
				GAMEMODE = 0;
				if (score >= hiscore[0])
				{
					if (score == hiscore[0] && rate > hirate)
					{
						hiscore[0] = score;
						hiscore[1] = true_score;
						hiscore[2] = false_score;
						hirate = rate;
						writer.open(U"Hiscore.txt");
						for (int i = 0; i < 3; ++i)writer << hiscore[i];
					}
					else
					{
						hiscore[0] = score;
						hiscore[1] = true_score;
						hiscore[2] = false_score;
						hirate = rate;
						writer.open(U"Hiscore.txt");
						for (int i = 0; i < 3; ++i)writer << hiscore[i];
					}


				}
			}
			break;

		}
			
	}


}


void QBox(int x, int y, Point Ratio)
{
	RoundRect(x-350 * Ratio.x, y-75 * Ratio.y, 700 * Ratio.x, 150 * Ratio.y, 10).draw(Palette::White).drawFrame(3, 0, Palette::Black);
	Rect(x - 20 * Ratio.x, y + 75 * Ratio.y, 40 * Ratio.x, 10 * Ratio.y).draw(Palette::Black);
	Rect(x - 10 * Ratio.x, y + 75 * Ratio.y, 20 * Ratio.x, 30 * Ratio.y).draw(Palette::Black);
}

void PowerBox(int x, int y, double Power, Point Ratio)
{
	const Vec2 center = { x, y + 70 * Ratio.y };
	RoundRect(x - 150 * Ratio.x, y - 75 * Ratio.y, 300, 150 * Ratio.y, 10).draw(Palette::Black);
	Circle(center, 140).drawPie(270_deg, 162_deg).drawPie(72_deg, 18_deg, Palette::Orangered);
	Line(center, center + Circular(140, Power * 1.8_deg - 90_deg)).drawArrow(5, Vec2(30, 30), Palette::Red);
	Rect(x - 20 * Ratio.x, y - 75 * Ratio.y, 40, -10 * Ratio.y).draw(Palette::Black);
	Rect(x - 10 * Ratio.x, y - 75 * Ratio.y, 20, -30 * Ratio.y).draw(Palette::Black);
}

void MakeQuestion(int& FirstTerm, int& SecondTerm, String& OperatorSymbol, int& AnswerTerm, bool& MistakeFlag)
{
	FirstTerm = Random(1, 99);
	SecondTerm = Random(1, 99);

	switch (Random(0,3))
	{
	case 0:
		OperatorSymbol = U"+";
		AnswerTerm = FirstTerm + SecondTerm;
		break;

	case 1:
		OperatorSymbol = U"-";
		AnswerTerm = FirstTerm - SecondTerm;
		break;

	case 2:
		OperatorSymbol = U"*";
		AnswerTerm = FirstTerm * SecondTerm;
		break;

	case 3:
		OperatorSymbol = U"/";
		AnswerTerm = FirstTerm / SecondTerm;
		break;
	}

	if (RandomBool())
	{
		if(RandomBool())AnswerTerm += Random(1, 10);
		else  AnswerTerm -= Random(1, 10);
		MistakeFlag = true;

	}

	else MistakeFlag = false;

}

void PowerCal(double& Power, Point Momentum)
{
	Power += Sqrt(Pow(Momentum.x, 2.0) + Pow(Momentum.y, 2.0)) / 200.0 - 1.0;

	if (Power < 0)Power = 0.0;
	else if (Power > 100.0)Power = 100.0;
}

void OpenQuest(int x, int y, int FirstTerm, int SecondTerm, String OperatorSymbol, int AnswerTerm, double Power, Font font)
{
	if (Power >= 90.0)
		font(FirstTerm, U" ", OperatorSymbol, U" ", SecondTerm, U" = ", AnswerTerm).drawAt(x, y, Palette::Black);

}

void PowerCable(int x, int yTop, int yBottom, double Power, Array<int>& yElect,Point Ratio)
{

	Line(x, yTop + 105 * Ratio.y, x, yBottom - 105 * Ratio.y).draw(Palette::Chocolate);
	
	
	if (yElect)
	{
		if (Power >= 90 && 20 < yBottom - 105 * Ratio.y - yElect.back())yElect << yBottom - 105 * Ratio.y;
		if (yElect.front() < yTop + 105 * Ratio.y)yElect.pop_front();
		for (int i = 0; i < yElect.size(); ++i)
		{
			Shape2D::NStar(7, 10, 5, Vec2(x, yElect[i])).draw(Palette::Gold);
			yElect[i]--;
		}
	}
	else if (Power >= 90)yElect << yBottom - 105 * Ratio.y;



}