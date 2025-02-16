#ifndef MEMEFIELD_H
#define MEMEFIELD_H

#include <assert.h>
#include <random>
#include "Graphics.h"
#include "SpriteCodex.h"
#include "Vei2.h"

class MemeField 
{
public:
	enum class State
	{
		Fucked,
		Winrar,
		Memeing
	};

private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Revealed,
			Flagged
		};

	public:
		void SpawnMeme();
		bool HasMeme() const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		void Draw(Graphics& gfx, MemeField::State state, const Vei2& screenPos) const;
		void SetNeighborMemeCount(int nMemeCount);

	private:
		bool hasMeme{ false };
		State state{ State::Hidden };
		int nNeighborMemes = -1; // -1 means uninitialized
	};


public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
	Vei2 GetStartPosition() const;
	static Vei2 GetCenterPositionPixels();
	State GetState() const;

private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2& screenPos) const;
	int CountNeighborMemes(const Vei2& gridPos) const;
	bool IsGameWon() const;

private:
	static constexpr int width = 5;
	static constexpr int height = 5;
	Tile field[width * height];
	State state = State::Memeing;

};

#endif