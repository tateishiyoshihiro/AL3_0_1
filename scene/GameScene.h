#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {
	//BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;
	//ビュープロジェクション(共通)
	ViewProjection viewProjection_  ;
	//ステージ
	uint32_t texttureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];
	//スコア数値(スプライト)
	uint32_t textureHandleNumber_ = 0;
	uint32_t textureHandleNm_ = 0;
	Sprite* spriteNumber_[5] = {};
	Sprite* spriteNm_ = nullptr;
	//
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = 0;
	WorldTransform worldTransformPlayer_;
	// ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = 0;
	WorldTransform worldTransformBeam_[10];

	//敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = 0;
	WorldTransform worldTransformEnemy_[10];

	//タイトル
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	// サブタイトル
	uint32_t textureHandlesabuTitle_ = 0;
	Sprite* spritesabuTitle_ = nullptr;
	
	//エンター
	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	//ゲームオーバー
	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;

	//サウンド
	uint32_t soundDataHandleTitleBGM_ = 0;		//タイトルBGM
	uint32_t soundDataHandleGamePlayBGM_ = 0;	//ゲームプレイBGM
	uint32_t soundDataHandleGameOverBGM_ = 0;	//ゲームオーバーBGM
	uint32_t soundDataHandleEnemyHitSE_ = 0;	//敵ヒットSE
	uint32_t soundDataHandlePlayerHitSE_ = 0;	//プレイヤーヒットSE
	uint32_t voiceHandleBGM_ = 0;				//音声再生ハンドル

	void GamePlayUpdate();
	void GamePlayDraw3D();
	void GamePlayDraw2DBack();
	void GamePlayDraw2DNear();
	void GamePlayStart();
	void PlayerUpdate();
	void TitleUpdate();
	void TitleDraw2DNear();
	void GameOverUpdate();
	void GameOverDraw2DNear();
	void DrawScore();
	int playerFlag_ = 0;
	void BeamUpdate();
	void BeamBorn();
	int beamFlag_[10] = {};
	void EnemyUpdete();
	void EnemyMove();
	void EnemyBorn();
	void EnemyJump();
	void StageUpdate();
	float enemyJumpSpeed_[10] = {};
	int enemyFlag_[10] = {};
	float enemySpeed_[10] = {};
	void Collision();
	void CollisionPlayerEnemy();
	void CollisionBeamEnemy();
	int gameScore_ = 0;//ゲームスコア
	int playerLife_ = 3;
	Sprite* spriteLife_[3] = {}; // プレイヤーライフ
	int sceneMode_ = 1;
	int gameTimer_ = 0;//ゲームタイマー
	int beamTimer_ = 0;
	int playerTimer_; // プレイヤータイマー

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
