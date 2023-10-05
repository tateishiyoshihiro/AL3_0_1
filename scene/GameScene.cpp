#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"MathUtilityForText.h"
//#include"MathUtilityForText.h"

//コントラスト
GameScene::GameScene() { 
	delete spriteBG_;//BG
	delete modelStage_;//Stage
}
//デストラクタ
GameScene::~GameScene() {
	delete modelPlayer_;
	delete spriteTitle_;//タイトル
	delete spriteEnter_;
}

void GameScene::Initialize() {
	srand((unsigned int)time(NULL));
	//BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});
	//ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	//ステージ
	texttureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	//ステージの数ループ
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
		// ステージの位置を変更
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f*i-5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int e = 0; e < 10; e++) {
		worldTransformBeam_[e].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformBeam_[e].Initialize();
		worldTransformBeam_[e].translation_.x = -20;
		worldTransformBeam_[e].translation_.y = -20;
		worldTransformBeam_[e].translation_.z = -20;
	}
	//敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();

	}

	//サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");
	 
	//タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
	
	//デバッグテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	//タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});
	//エンター
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});
	//ゲームオーバー
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {10, 100});
	//スコア数値(2Dスプライト)
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}
	// ライフ
	for (int i = 0; i < 3; i++) {
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 10});
		spriteLife_[i]->SetSize({40, 40});
	}
	//スコアの文字
	textureHandleNm_ = TextureManager::Load("score.png");
	spriteNm_ = Sprite::Create(textureHandleNm_, {100.0f, 5.0f});
}

// 更新  worldTransformBeam_.translation_.z = -20;
void GameScene::GamePlayStart() { 
	textureHandleNumber_ = 0;
	playerLife_ = 3;
	playerTimer_ = 0;
	for (int e = 0; e < 10; e++) {
		for (int i = 0; i < 10; i++) {
			beamFlag_[e] = 0;
			enemyFlag_[i]=0;
		}
	}
	GamePlayUpdate();
}
//
//
//
void GameScene::GamePlayUpdate() {
	PlayerUpdate();
	EnemyUpdete();
	BeamUpdate();
	Collision();
	StageUpdate();
	// エンターキーを押したとき
	if (playerLife_ == 0) {
		// モードをゲームプレイへ変更
		sceneMode_ = 2;

		//サウンド切り替え
		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOverBGM_);
	}
}
void GameScene::Update() {
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate();
		break;
	case 1:
		TitleUpdate();
		break;
	case 2:
		GameOverUpdate();
		break;
	}
}
    //
void GameScene::GamePlayDraw2DBack() {
	//
	spriteBG_->Draw();
}
//
void GameScene::GamePlayDraw2DNear() {
	DrawScore();
	// ゲームスコア
	spriteNm_->Draw();
	// ライフ
	for (int i = 0; i < playerLife_; i++) {
		spriteLife_[i]->Draw();
	}
}
//スコア数値の表示
void GameScene::DrawScore() {
	//各桁の値を取り出す
	int eachNumber[5] = {};//各桁の値
	int number = gameScore_; // 表示する数字

	int keta = 10000;//最初の桁
	for (int i = 0; i < 5; i++) {
		eachNumber[i] = number / keta;//今の桁の値を求める
		number = number % keta;//次の桁以下の値を取り戻す
		keta = keta / 10;//桁を進める
	}
	//各桁の数値を描画
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}
}
    //敵ジャンプ
void GameScene::EnemyJump() {
	//敵ループ
	for (int i = 0; i < 10; i++) {
	//消滅演出ならば
		if (enemyFlag_[i] == 2) {
			//移動(Y座標に速度を加える)
			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];
			//速度を減らす
			enemyJumpSpeed_[i] -= 0.1f;
			//斜め移動
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i] * 4;
			//下へ落ちると消滅
			if (worldTransformEnemy_[i].translation_.y<-3) {
				enemyFlag_[i] = 0;
			}
		}
	}
}
 //プレイヤー更新
void GameScene::PlayerUpdate() {
	
	//移動
	// 
	//右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
	worldTransformPlayer_.translation_.x+=0.1f;
	}
	if (worldTransformPlayer_.translation_.x >= 4) {
	worldTransformPlayer_.translation_.x = 4;
	}
	//左へ移動
	if (input_->PushKey(DIK_LEFT)) {
	worldTransformPlayer_.translation_.x -= 0.1f;
	}
	if (worldTransformPlayer_.translation_.x <= -4) {
	worldTransformPlayer_.translation_.x = -4;
	}
	//変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	//変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();

	worldTransformPlayer_.rotation_.y += 0.1f;
	if (playerTimer_ > 0) {
	playerTimer_ -= 1;
	}
}
//ビームはここから

void GameScene::BeamUpdate() {
	BeamBorn();
	for (int e = 0; e < 10; e++) {
			if (beamFlag_[e] == 1) {
				worldTransformBeam_[e].translation_.z += 0.5f;
				if (worldTransformBeam_[e].translation_.z >= 40) {
					worldTransformBeam_[e].translation_.x = 10;
					worldTransformBeam_[e].translation_.y = 10;
					worldTransformBeam_[e].translation_.z = -20;
					beamFlag_[e] = 0;
				}
			}
			//
			worldTransformBeam_[e].matWorld_ = MakeAffineMatrix(
			worldTransformBeam_[e].scale_, worldTransformBeam_[e].rotation_,
			worldTransformBeam_[e].translation_);
			worldTransformBeam_[e].TransferMatrix();

			// 回転
			worldTransformBeam_[e].rotation_.x += 0.1f;
		}
	}

void GameScene::BeamBorn() { 
	// 発射タイマーが0ならば
	if (beamTimer_ == 0) {
		    for (int e = 0; e < 10; e++) {
			    if (input_->PushKey(DIK_SPACE)) {
				    if (beamFlag_[e] == 0) {
					    worldTransformBeam_[e].translation_.x =
					        worldTransformPlayer_.translation_.x;
					    worldTransformBeam_[e].translation_.y =
					        worldTransformPlayer_.translation_.y;
					    worldTransformBeam_[e].translation_.z =
					        worldTransformPlayer_.translation_.z;
					    beamFlag_[e] = 1;
					   beamTimer_ = 1;
					    break;
				    }
			    }
		    }
		
	} 
	else {
	 beamTimer_++;
	 if (beamTimer_ > 10) {
		beamTimer_ = 0;
	 }
	}
}
void GameScene::EnemyUpdete() {
	EnemyMove();
	EnemyBorn();
	EnemyJump();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		worldTransformEnemy_[i].translation_);
		worldTransformEnemy_[i].TransferMatrix();
	};
}
//タイトル更新
void GameScene::TitleUpdate() {
	//エンターキーを押したとき
	if (input_->TriggerKey(DIK_RETURN)) {
		//モードをゲームプレイへ変更
		sceneMode_ = 0;
		//BGM切り替え
		audio_->StopWave(voiceHandleBGM_);//現在のBGMを停止
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
	}
}
void GameScene::TitleDraw2DNear() {
	gameTimer_++;
	if (gameTimer_ >= 40) {
		gameTimer_ = 0;
	}

	//タイトル表示
	spriteTitle_->Draw();
	//エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}
//ステージ更新
void GameScene::StageUpdate() {
	//各ステージでループ
	for (int i = 0; i < 20; i++) {
	//手前に移動
		worldTransformStage_[i].translation_.z -= 0.1f;
		//端まで来たら奥へ戻る
		if (worldTransformStage_[i].translation_.z < -5) {
		worldTransformStage_[i].translation_.z += 40;
		}
		//変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		//変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
}

void GameScene::GameOverUpdate() {
	//エンターキーを押したとき
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		for (int i = 0; i < 10; i++) {
			for (int e = 0; e < 10; e++) {
				sceneMode_ = 1;
				gameScore_ = 0;
				beamFlag_[e] = 0;
				enemyFlag_[i] = 0;
				worldTransformPlayer_.translation_.x = 0;
				if (playerLife_ == 0) {
					playerLife_ = 3;
					GamePlayStart();
					audio_->StopWave(voiceHandleBGM_);
					voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_);
				}
			}
		}
	}
}

void GameScene::GameOverDraw2DNear() {
	gameTimer_++;
	if (gameTimer_ >= 40) {
		gameTimer_ = 0;
	}
	// タイトル表示
	spriteGameOver_->Draw();
	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::EnemyMove() {
	for (int i = 0; i < 10; i++) {

		if (enemyFlag_[i] == 1) {
			worldTransformEnemy_[i].translation_.z -= 0.5f;
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i];
			if (worldTransformEnemy_[i].translation_.x >= 4) {
				enemySpeed_[i] =- 0.1f;
			} 
			if (worldTransformEnemy_[i].translation_.x <= -4) {
				enemySpeed_[i] = 0.1f;
			}
			
			if (worldTransformEnemy_[i].translation_.z <= -5) {
				
				worldTransformEnemy_[i].translation_.y = 0;
				worldTransformEnemy_[i].translation_.z = 40;

				enemyFlag_[i] = 0;
			}
		}
		worldTransformEnemy_[i].translation_.z -= 0.1f;
		worldTransformEnemy_[i].translation_.z -= gameTimer_ / 1000.0f;
		worldTransformEnemy_[i].rotation_.x -= 0.2f;
	}
}
	
void GameScene::EnemyBorn() { 
	if (rand() % 10 == 0) {
		//発射タイマーが0ならば
		for (int i = 0; i < 10; i++) {
	
			if (enemyFlag_[i] == 0) {
				worldTransformEnemy_[i].translation_.x = 0;
				worldTransformEnemy_[i].translation_.y = 0;
				worldTransformEnemy_[i].translation_.z = 40;
				enemyFlag_[i] = 1;
				if (rand()%2==0) {
					enemySpeed_[i] = 0.1f;
				} else {
					enemySpeed_[i] = -0.1f;
				}
				// 乱数でX座標の指定
			
				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;
				worldTransformEnemy_[i].translation_.x = x2;
				worldTransformEnemy_[i].translation_.y = 0;
				break;
			
			}
		}
	}
}

//衝突判定
void GameScene::Collision() {
	//衝突判定(プレイヤーと敵)
	CollisionPlayerEnemy();
	//衝突判定(敵と弾)
	CollisionBeamEnemy();
}

//衝突判定(プレイヤーと敵)
void GameScene::CollisionPlayerEnemy() {
	//
	
	for (int i = 0; i < 10; i++) {
	//
		
			// 敵が存在すれば
			if (enemyFlag_[i] == 1) {
				// 差を求める
				float dx = abs(
					worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
				float dz = abs(
					worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);

				// 衝突したら
				if (dx < 1 && dz < 1) {
					// 存在しない
					enemyFlag_[i] = 0;
					playerLife_--;
				    playerTimer_ = 60;

					//プレイヤーヒットSE
				    audio_->PlayWave(soundDataHandleEnemyHitSE_);
				}
			}
		}
	}
	

void GameScene::CollisionBeamEnemy() {
	    for (int i = 0; i < 10; i++) {
		    for (int e = 0; e < 10; e++) {
				if (enemyFlag_[i] == 1 && beamFlag_[e] == 1) {
				    float ex =
				        abs(worldTransformBeam_[e].translation_.x -
				            worldTransformEnemy_[i].translation_.x);
				    float ez =
				        abs(worldTransformBeam_[e].translation_.z -
				            worldTransformEnemy_[i].translation_.z);
				    // 衝突したら
				    if (ex < 1 && ez < 1) {
					    // 存在しない
					    enemyFlag_[i] = 2;
					    beamFlag_[e] = 0;
					    enemyJumpSpeed_[i] = 1;
					    gameScore_ += 100;

						//敵ヒットSE
					    audio_->PlayWave(soundDataHandleEnemyHitSE_);
				    }
			    }
		    }
	    }
    }
    void GameScene::GamePlayDraw3D() {
//
	    for (int i = 0; i < 20; i++) {
		    modelStage_->Draw(worldTransformStage_[i], viewProjection_, texttureHandleStage_);
	    }
	//
	    if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	    } 

		for (int e = 0; e < 10; e++) {
		    if (beamFlag_[e] == 1) {
			    modelBeam_->Draw(worldTransformBeam_[e], viewProjection_, textureHandleBeam_);
		    }
		}
	//
	for (int i = 0; i < 10; i++) {
	for (int e = 0; e < 10; e++) {
		if (enemyFlag_[i] >= 1) {
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
				    if (enemyFlag_[i] == 2)
					    modelEnemy_->Draw(
					        worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
			break;
		}
	}
	}
}

    //描画
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// 
	
	switch (sceneMode_) {
	case 0:
	GamePlayDraw2DBack();
	break;
	case 2:
	GamePlayDraw2DBack();
	break;
	}
	

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	//ステージ

	/// </summary>
	switch (sceneMode_) {
	case 0:
	GamePlayDraw3D();
	break;
	case 2:
	GamePlayDraw3D();
	break;
	}
	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) {
	case 0:
	GamePlayDraw2DNear();
	break;
	case 1:
	TitleDraw2DNear();
	break;
	case 2:
	GamePlayDraw2DNear();
	GameOverDraw2DNear();
	}
	debugText_->DrawAll();
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
