#ifndef PACSNODE_H
#define PACSNODE_H

/**
 * @file PacsNode.h
 * @brief Declares the PacsNode entity describing a PACS endpoint (AE) and connection info.
 *
 * Represents a DICOM Application Entity configuration (e.g., Archive, MWL, MPPS),
 * including host/network coordinates and AE titles used for association negotiation.
 */

#include "PacsEntityType.h"
#include <QDate>       // (Optional) Currently unused; remove if unnecessary.
#include <QDateTime>   // (Optional) Currently unused; remove if unnecessary.
#include <QMetaType>
#include <QString>

 /// PACS data-layer entities for Etrek.
 /**
  * @namespace Etrek::Pacs::Data::Entity
  * @brief Persistable domain objects for PACS configuration and metadata.
  */
namespace Etrek::Pacs::Data::Entity {

    namespace pks = Etrek::Pacs;

    /**
     * @class PacsNode
     * @brief Persistable entity describing a DICOM PACS node/Application Entity.
     *
     * A PacsNode captures the remote (or local) DICOM peer endpoint used by the
     * application, including AE titles and TCP parameters. Equality compares by
     * the surrogate key (@ref Id) only.
     */
    class PacsNode {
    public:
        /**
         * @brief Surrogate primary key for persistence.
         *
         * Defaults to -1 to indicate a transient/unsaved instance.
         */
        int Id = -1;

        /**
         * @brief PACS entity role/type (e.g., Archive, MPPS, MWL, C-STORE SCP).
         */
        pks::PacsEntityType Type = pks::PacsEntityType::Archive;

        /**
         * @brief DNS host name of the AE (e.g., "pacs01.example.org").
         *
         * For IPv4/IPv6 literals, prefer using @ref HostIp and keep this as
         * a friendly label; otherwise ensure it resolves to the endpoint.
         */
        QString HostName;

        /**
         * @brief IP address of the AE (IPv4 or IPv6).
         *
         * When both @ref HostName and @ref HostIp are present, runtime logic
         * should define which takes precedence (typically explicit IP).
         */
        QString HostIp;

        /**
         * @brief TCP port used by the AE (default often 104, or site-specific).
         */
        int Port = 0;

        /**
         * @brief Called AE Title (the peer AE title you are connecting to).
         *
         * This must match the configuration on the remote SCP/SCP-AE.
         */
        QString CalledAet;

        /**
         * @brief Calling AE Title (your local AE title presented to the peer).
         *
         * Typically unique per application instance; used by the remote to
         * authorize/route associations.
         */
        QString CallingAet;

        /**
         * @brief Default constructor.
         */
        PacsNode() = default;

        /**
         * @brief Equality operator comparing by primary key only.
         * @param other Another PacsNode to compare with.
         * @return true if both nodes have the same @ref Id; otherwise false.
         *
         * @note Network coordinates or titles are not compared here.
         */
        bool operator==(const PacsNode& other) const { return Id == other.Id; }
    };

} // namespace Etrek::Pacs::Data::Entity

/**
 * @brief Enables PacsNode for use with Qt's meta-object system (e.g., QVariant).
 */
Q_DECLARE_METATYPE(Etrek::Pacs::Data::Entity::PacsNode)

#endif // !PACSNODE_H
