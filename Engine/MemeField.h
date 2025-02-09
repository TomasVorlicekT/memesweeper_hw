#ifndef MEMEFIELD_H
#define MEMEFIELD_H

#include <assert.h>
#include <random>
#include "Graphics.h"
#include "SpriteCodex.h"
#include "Vei2.h"

class MemeField 
{
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
		void Draw(Graphics& gfx, const Vei2& screenPos) const;

	private:
		bool hasMeme{ false };
		State state{ State::Hidden };
	};

public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);

private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2& screenPos) const;

private:
	static constexpr int width = 16;
	static constexpr int height = 20;
	Tile field[width * height];

};

#endif