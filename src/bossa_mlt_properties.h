/*
 * Copyright (c) 2013-2025 Bossa Project, LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHOTCUT_MLT_PROPERTIES_H
#define SHOTCUT_MLT_PROPERTIES_H

/* This file contains all of the Bossa-specific MLT properties.
 * See also https://www.bossa.org/notes/mltxml-annotations/
 *
 * A property should be prefaced with an underscore if it will not be saved
 * in the XML even if it never has a chance of getting into there. This makes
 * it more clear which is also an XML annotation or purely internal use.
 */

/* MLT XML annotations */

#define kBossaXmlProperty "bossa"
#define kAudioTrackProperty "shotcut:audio"
#define kCommentProperty "shotcut:comment"
#define kBossaFilterProperty "shotcut:filter"
#define kBossaPlaylistProperty "shotcut:playlist"
#define kBossaTransitionProperty "shotcut:transition"
#define kBossaProducerProperty "shotcut:producer"
#define kBossaVirtualClip "shotcut:virtual"
#define kTimelineScaleProperty "shotcut:scaleFactor"
#define kTrackHeightProperty "shotcut:trackHeight"
#define kTrackHeaderWidthProperty "shotcut:trackHeaderWidth"
#define kTrackNameProperty "shotcut:name"
#define kTrackLockProperty "shotcut:lock"
#define kVideoTrackProperty "shotcut:video"
#define kBossaCaptionProperty "shotcut:caption"
#define kBossaDetailProperty "shotcut:detail"
#define kBossaHashProperty "shotcut:hash"
#define kBossaHiddenProperty "shotcut:hidden"
#define kBossaSkipConvertProperty "shotcut:skipConvert"
#define kBossaAnimInProperty "shotcut:animIn"
#define kBossaAnimOutProperty "shotcut:animOut"
#define kBossaMarkersProperty "shotcut:markers"
#define kBossaGroupProperty "shotcut:group"
// Bossa's VUI (video user interface) components set this so that glwidget can
// hide the VUI when the play head is not over the clip with the current filter.
#define kBossaVuiMetaProperty "meta.shotcut.vui"
#define kDefaultAudioIndexProperty "shotcut:defaultAudioIndex"
#define kOriginalResourceProperty "shotcut:resource"
#define kOriginalInProperty "shotcut:originalIn"
#define kOriginalOutProperty "shotcut:originalOut"
#define kDisableProxyProperty "shotcut:disableProxy"
#define kBackupProperty "shotcut:backup"
// "shotcut:proxy" is internal only because older versions do not know to hide it.
// "shotcut:metaProxy" indicates whether the "meta." properties reflect source or proxy.
#define kMetaProxyProperty "shotcut:proxy.meta"
#define kBossaBinsProperty "shotcut:bins"

/* Project specific properties */
#define kBossaProjectAudioChannels "shotcut:projectAudioChannels"
#define kBossaProjectFolder "shotcut:projectFolder"
#define kBossaProjectNote "shotcut:projectNote"
#define kBossaProjectProcessingMode "shotcut:processingMode"

/* Ideally all shotcut properties should begin with "shotcut:", but these
 * do not and kept for legacy reasons? */

#define kAspectRatioNumerator "shotcut_aspect_num"
#define kAspectRatioDenominator "shotcut_aspect_den"
#define kBossaSequenceProperty "shotcut_sequence"

/* Special object Ids expected by Bossa and used in XML */

#define kBackgroundTrackId "background"
#define kLegacyPlaylistTrackId "main bin"
#define kPlaylistTrackId "main_bin"

/* Internal only */

#define kAudioLevelsProperty "_shotcut:audio-levels"
#define kBackgroundCaptureProperty "_shotcut:bgcapture"
#define kPlaylistIndexProperty "_shotcut:playlistIndex"
#define kPlaylistStartProperty "_shotcut:playlistStart"
#define kFilterInProperty "_shotcut:filter_in"
#define kFilterOutProperty "_shotcut:filter_out"
#define kThumbnailInProperty "_shotcut:thumbnail-in"
#define kThumbnailOutProperty "_shotcut:thumbnail-out"
#define kUuidProperty "_shotcut:uuid"
#define kMultitrackItemProperty "_shotcut:multitrack-item"
#define kExportFromProperty "_shotcut:exportFromDefault"
#define kTrackIndexProperty "_shotcut:trackIndex"
#define kFilterIndexProperty "_shotcut:filterIndex"
#define kNewFilterProperty "_shotcut:newFilter"
#define kBossaFiltersClipboard "shotcut:filtersClipboard"
#define kIsProxyProperty "shotcut:proxy"
#define kPrivateProducerProperty "_shotcut:producer"
#define kNewFrameOutProperty "_shotcut:newFrameOut"

#define kDefaultMltProfile "atsc_1080p_25"

#endif // SHOTCUT_MLT_PROPERTIES_H
