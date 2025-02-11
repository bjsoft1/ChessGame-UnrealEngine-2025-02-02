#pragma once

UENUM()
enum class EChessPlayers : uint8
{
	/// <summary>
	/// Only view access
	/// </summary>
	Guest = 0,

	/// <summary>
	/// Blank Player
	/// </summary>
	Black = 1,

	/// <summary>
	/// White Player
	/// </summary>
	White = 2
};
UENUM()
enum class EChessPieceTypes : uint8
{
    /// <summary>
    /// Army
    /// </summary>
    Pawn,
    /// <summary>
    /// Elaphant
    /// </summary>
    Rook,
    /// <summary>
    /// Hourse
    /// </summary>
    Knight,
    /// <summary>
    /// Camel
    /// </summary>
    Bishop,
    /// <summary>
    /// Minister
    /// </summary>
    Queen,
    /// <summary>
    /// King
    /// </summary>
    King,
};
