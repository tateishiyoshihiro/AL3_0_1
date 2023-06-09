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
}

void GameScene::Initialize() {
	//BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});
	//ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	//ステージ
	texttureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();
	//ステージの位置を変更
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	//変換行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, 
		worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);
	//変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();

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
	worldTransformBeam_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformBeam_.Initialize();

	worldTransformBeam_.translation_.x = -20;
	worldTransformBeam_.translation_.y = -20;
	worldTransformBeam_.translation_.z = -20;

	//敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.Initialize();
	worldTransformEnemy_.translation_.z = -40;

	//デバッグテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	//タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});
}

// 更新  worldTransformBeam_.translation_.z = -20;

//
//
//
void GameScene::GamePlayUpdate() {
	PlayerUpdate();
	EnemyUpdete();
	BeamUpdate();
	Collision();
}
void GameScene::Update() {
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate();
		break;
	case 1:
		TitleUpdate();
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
	// ゲームスコア
	char str[100];
	
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);
	sprintf_s(str, "LIFE %d", playerLife_);
	debugText_->Print(str, 900, 10, 2);

	
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
}
//ビームはここから

void GameScene::BeamUpdate() { 
	if (input_->PushKey(DIK_SPACE)) {
	if (beamFlag_ == 1)
	{
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.y = worldTransformPlayer_.translation_.y;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
		beamFlag_ = 0;
	}
	}
	if (beamFlag_ == 0) {
	worldTransformBeam_.translation_.z += 0.5f;
	if (worldTransformBeam_.translation_.z >= 40) {
		worldTransformBeam_.translation_.x = 10;
		worldTransformBeam_.translation_.y = 10;
		worldTransformBeam_.translation_.z = -20;
		beamFlag_ = 1;
	}
	}
	//
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
		worldTransformBeam_.scale_,
		worldTransformBeam_.rotation_,
		worldTransformBeam_.translation_
	);
	worldTransformBeam_.TransferMatrix();

	//回転
	worldTransformBeam_.rotation_.x += 0.1f;

}
void GameScene::EnemyUpdete() {
	EnemyMove();
	EnemyBorn();
	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_,
		worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);
	worldTransformEnemy_.TransferMatrix();
}
void GameScene::TitleUpdate() {

}
void GameScene::TitleDraw2DNear() {

	//タイトル表示
	spriteTitle_->Draw();
}

void GameScene::EnemyMove() {
	if (enemyFlag_ == 1) {
	worldTransformEnemy_.translation_.z -= 0.5f;
	if (worldTransformEnemy_.translation_.z <= -5)
	{
		worldTransformEnemy_.translation_.x =	0;
		worldTransformEnemy_.translation_.y = 0;
		worldTransformEnemy_.translation_.z = 40;

		enemyFlag_ = 0;
	}
	}
	worldTransformEnemy_.rotation_.x -= 0.1f;
	}
void GameScene::EnemyBorn() { 
	if (enemyFlag_ == 0) {
	worldTransformEnemy_.translation_.x = 0;
	worldTransformEnemy_.translation_.y = 0;
	worldTransformEnemy_.translation_.z = 40;
	enemyFlag_ = 1;
	// 乱数でX座標の指定
	int x = rand() % 80;
	float x2 = (float)x / 10 - 4;
	worldTransformEnemy_.translation_.x = x2;
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
	//敵が存在すれば
	if (enemyFlag_ == 1) {
	//差を求める
	float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
	float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);

	//衝突したら
	if (dx < 1 && dz < 1) {
	//存在しない
		enemyFlag_ = 0;
		playerLife_--;
	}
	}
}
void GameScene::CollisionBeamEnemy() { 
	if (enemyFlag_ == 1&&beamFlag_==0) {
	float ex = abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
	float ez = abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);
	// 衝突したら
	if (ex < 1 && ez < 1) {
		// 存在しない
		enemyFlag_ = 0;
		beamFlag_ = 1;
		gameScore_++;
	}
	}
}
void GameScene::GamePlayDraw3D() {
//
	modelStage_->Draw(worldTransformStage_, viewProjection_, texttureHandleStage_);
	//
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	//
	if (beamFlag_ == 0) {
	modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	//
	if (enemyFlag_ == 1) {
	modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
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
	 debugText_->DrawAll();
	switch (sceneMode_) {
	case 0:
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
	}
	switch (sceneMode_) {
	case 0:
	GamePlayDraw2DNear();
	break;
	case 1:
	TitleDraw2DNear();
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

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
