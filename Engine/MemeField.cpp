#include "MemeField.h"

void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Reveal()
{
	assert(state == State::Hidden);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

void MemeField::Tile::ToggleFlag()
{
	assert(state == State::Hidden || state == State::Flagged);

	if (state == State::Hidden)
	{
		state = State::Flagged;
	}
	else if (state == State::Flagged)
	{
		state = State::Hidden;
	}
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

void MemeField::Tile::Draw(Graphics& gfx, const Vei2& screenPos) const
{
	switch (state)
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		break;

	case State::Flagged:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		SpriteCodex::DrawTileFlag(screenPos, gfx);
		break;

	case State::Revealed:
		if (hasMeme)
		{
			SpriteCodex::DrawTileBomb(screenPos, gfx);
		}
		else
		{
			SpriteCodex::DrawTile0(screenPos, gfx);
		}
		break;

	}
}

MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width * height);

	std::random_device rd{};
	std::mt19937 rng{ rd() };
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int nSpawned = 0; nSpawned < nMemes; nSpawned++)
	{
		Vei2 spawnPos = { xDist(rng), yDist(rng) };
		do
		{
			spawnPos.x = xDist(rng);
			spawnPos.y = yDist(rng);
		} while (TileAt(spawnPos).HasMeme());

		TileAt(spawnPos).SpawnMeme();
	}

	//for (int reveal = 0; reveal < 50; reveal++)
	//{
	//	Vei2 revealLoc = { xDist(rng), yDist(rng) };
	//	if (!TileAt(revealLoc).IsRevealed())
	//	{
	//		TileAt(revealLoc).Reveal();
	//	};
	//}

}

void MemeField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);

	for (Vei2 startPos = { 0, 0 }; startPos.y < height; startPos.y++)
	{
		for (startPos.x = 0; startPos.x < width; startPos.x++)
		{
			Vei2 gridPositionCurrent = { startPos.x, startPos.y };
			TileAt(gridPositionCurrent).Draw(gfx, gridPositionCurrent * SpriteCodex::tileSize);
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI(0, width * SpriteCodex::tileSize, 0, height * SpriteCodex::tileSize);
}

void MemeField::OnRevealClick(const Vei2& screenPos)
{
	RectI playScreenArea = GetRect();
	assert( screenPos.x >= playScreenArea.left &&
			screenPos.x < playScreenArea.right &&
			screenPos.y >= playScreenArea.top &&
			screenPos.y < playScreenArea.bottom);

	if (!TileAt(ScreenToGrid(screenPos)).IsRevealed() && !TileAt(ScreenToGrid(screenPos)).IsFlagged())
	{
		TileAt(ScreenToGrid(screenPos)).Reveal();
	}
}

void MemeField::OnFlagClick(const Vei2& screenPos)
{
	RectI playScreenArea = GetRect();
	assert(screenPos.x >= playScreenArea.left &&
		screenPos.x < playScreenArea.right &&
		screenPos.y >= playScreenArea.top &&
		screenPos.y < playScreenArea.bottom);

	if (!TileAt(ScreenToGrid(screenPos)).IsRevealed())
	{
		TileAt(ScreenToGrid(screenPos)).ToggleFlag();
	}
}

MemeField::Tile& MemeField::TileAt(const Vei2 & gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const MemeField::Tile& MemeField::TileAt(const Vei2& gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 MemeField::ScreenToGrid(const Vei2& screenPos) const
{
	return Vei2(screenPos / SpriteCodex::tileSize);
}
