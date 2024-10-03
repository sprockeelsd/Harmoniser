//
// Created by Damien Sprockeels on 18/09/2024.
//

#include "../headers/MusicalParts.hpp"

using namespace Gecode;

void tonal_progression(const Home &home, int size, IntVarArray &chords, IntVarArray &states, IntVarArray &qualities,
                       IntVarArray &isChromatic, IntVarArray &hasSeventh, IntVarArray &bassNotes,
                       IntVarArray &rootNotes, Tonality *tonality, int minChromaticChords, int maxChromaticChords,
                       int minSeventhChords, int maxSeventhChords) {
    ///1. chord[i] -> chord[i+1] is possible (matrix)
    chord_transitions(home, size, chords);

    ///2. The quality of each chord is linked to the degree it is (V is major/7, I is major,...)
    link_chords_to_qualities(home, size, chords, qualities);

    ///3. The state of each chord is linked to the degree it is (I can be in fund/1st inversion, VI can be in fund,...)
    link_chords_to_states(home, size, chords, states);

    ///4. The state of each chord is linked to its quality (7th chords can be in 3rd inversion, etc)
    link_states_to_qualities(home, size, states, hasSeventh);

    /// Link root notes
    root_notes(home, size, chords, rootNotes, tonality);

    ///5. link root note to chord + degree;
    link_bass_degrees_to_degrees_and_states(home, size, chords, states, bassNotes);

    ///6. Link the chromatic chords and count them so that there are exactly nChromaticChords
    chromatic_chords(home, size, chords, isChromatic, minChromaticChords, maxChromaticChords);

    ///7. Link the seventh chords and count them so that there are exactly nSeventhChords
    seventh_chords(home, size, hasSeventh, qualities, minSeventhChords, maxSeventhChords);

    //todo see if this is still relevant in this case. I think it should be in TonalPiece and not in ChordProgression, or at least only in the final tonal progression
//    ///7. The chord progression cannot end on something other than a diatonic chord (also not seventh degree) todo change that with the modulations?
//    last_chord_cst(home, size, chords);
//
    ///8. I64-> V5/7+ (same state)
    fifth_degree_appogiatura(home, size, chords, states, qualities);

    ///9. bII should be in first inversion todo maybe make this a preference?
    flat_II_cst(home, size, chords, states);

    ///10. If two successive chords are the same degree, they cannot have the same state or the same quality
    ///11. The same degree cannot happen more than twice successively
    successive_chords_with_same_degree(home, size, chords, states, qualities);

    ///12. Tritone resolutions should be allowed with the states todo rework this and test it extensively
    tritone_resolutions(home, size, chords, states);

    ///13. Fifth degree chord cannot be in second inversion if it is not dominant seventh
    fifth_degree(home, size, chords, states, qualities);

    /// cadences
    rel(home, chords[0], IRT_EQ, FIRST_DEGREE);
    cadence(home, size / 2, HALF_CADENCE, chords, states, hasSeventh);
    //cadence(home, startPosition + size - 2, PERFECT_CADENCE, chords, states, hasSeventh);
}