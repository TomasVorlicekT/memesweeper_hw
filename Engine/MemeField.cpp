#include "MemeField.h"
#include <algorithm>

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

void MemeField::Tile::Draw(Graphics& gfx, MemeField::State fieldState, const Vei2& screenPos) const
{
	if (fieldState != MemeField::State::Fucked)
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
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			break;

		}
	}
	else
	{
		switch (state)
		{
		case State::Hidden:
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;

		case State::Flagged:
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);

			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;

		case State::Revealed:
			if (hasMeme)
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			break;

		}
	}

}

void MemeField::Tile::SetNeighborMemeCount(int nMemeCount)
{
	assert(nNeighborMemes == -1);
	nNeighborMemes = nMemeCount;
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

	for (Vei2 startPos = { 0, 0 }; startPos.y < height; startPos.y++)
	{
		for (startPos.x = 0; startPos.x < width; startPos.x++)
		{
			Vei2 gridPositionCurrent = { startPos.x, startPos.y };
			int nMemes = CountNeighborMemes(gridPositionCurrent);
			TileAt(gridPositionCurrent).SetNeighborMemeCount(nMemes);
		}
	}

}

void MemeField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);

	for (Vei2 startPos = { 0, 0 }; startPos.y < height; startPos.y++)
	{
		for (startPos.x = 0; startPos.x < width; startPos.x++)
		{
			Vei2 gridPositionCurrent = { startPos.x, startPos.y };
			TileAt(gridPositionCurrent).Draw(gfx, state, (gridPositionCurrent + GetStartPosition()) * SpriteCodex::tileSize);
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI(
		GetStartPosition().x * SpriteCodex::tileSize, 
		(GetStartPosition().x + width) * SpriteCodex::tileSize, 
		GetStartPosition().y * SpriteCodex::tileSize,
		(GetStartPosition().y + height) * SpriteCodex::tileSize);
}

void MemeField::OnRevealClick(const Vei2& screenPos)
{
	RectI playScreenArea = GetRect();
	assert( screenPos.x >= playScreenArea.left &&
			screenPos.x < playScreenArea.right &&
			screenPos.y >= playScreenArea.top &&
			screenPos.y < playScreenArea.bottom);

	if (state == State::Memeing)
	{
		if (!TileAt(ScreenToGrid(screenPos)).IsRevealed() && !TileAt(ScreenToGrid(screenPos)).IsFlagged())
		{
			TileAt(ScreenToGrid(screenPos)).Reveal();

			if (TileAt(ScreenToGrid(screenPos)).HasMeme())
			{
				state = State::Fucked;
			}
			else if (IsGameWon())
			{
				state = State::Winrar;
			}
		}
	};
}

void MemeField::OnFlagClick(const Vei2& screenPos)
{
	RectI playScreenArea = GetRect();
	assert(screenPos.x >= playScreenArea.left &&
		screenPos.x < playScreenArea.right &&
		screenPos.y >= playScreenArea.top &&
		screenPos.y < playScreenArea.bottom);
	
	if (state == State::Memeing)
	{
		if (!TileAt(ScreenToGrid(screenPos)).IsRevealed())
		{
			TileAt(ScreenToGrid(screenPos)).ToggleFlag();
		}
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
	return Vei2(screenPos / SpriteCodex::tileSize) - GetStartPosition();
}

int MemeField::CountNeighborMemes(const Vei2& gridPos) const
{
	int count = 0;

	int xStart = std::max(0, gridPos.x - 1);
	int yStart = std::max(0, gridPos.y - 1);
	int xEnd = std::min(width - 1, gridPos.x + 1);
	int yEnd = std::min(height - 1, gridPos.y + 1);

	for (Vei2 start = { xStart, yStart }; start.y <= yEnd; start.y++)
	{
		for (start.x = xStart; start.x <= xEnd; start.x++)
		{
			if (TileAt(start).HasMeme())
			{
				count++;
			}
		}
	}

	return count;
}

Vei2 MemeField::GetStartPosition() const
{
	Vei2 startPos = { 
		((Graphics::ScreenWidth / SpriteCodex::tileSize) - width) / 2, 
		((Graphics::ScreenHeight / SpriteCodex::tileSize) - height) / 2 
	};

	return startPos;
}

// Checks whether all win conditions are met
bool MemeField::IsGameWon() const
{
	for (const Tile& tile : field)
	{
		if (!tile.HasMeme() && !tile.IsRevealed())
		{
			return false;
		}
	}

	return true;
}

Vei2 MemeField::GetCenterPositionPixels()
{
	return Vei2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2 );
}

MemeField::State MemeField::GetState() const
{
	return state;
}
